/*******************************************************************************
 * ** Copyright (c) 2006-201a The Krell Institue. All Rights Reserved.
 * **
 * ** This library is free software; you can redistribute it and/or modify it under
 * ** the terms of the GNU Lesser General Public License as published by the Free
 * ** Software Foundation; either version 2.1 of the License, or (at your option)
 * ** any later version.
 * **
 * ** This library is distributed in the hope that it will be useful, but WITHOUT
 * ** ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * ** FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
 * ** details.
 * **
 * ** You should have received a copy of the GNU Lesser General Public License
 * ** along with this library; if not, write to the Free Software Foundation, Inc.,
 * ** 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 * *******************************************************************************/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#if defined(RUNTIME_PLATFORM_BGP) || defined(RUNTIME_PLATFORM_BGQ)
#define BLUEGENE 1
#endif

#define _ISOC99_SOURCE

#ifdef HAVE_INTTYPES_H
#include <inttypes.h>
#endif

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <limits.h>
#include <unistd.h>
#include <sys/types.h>
#include <errno.h>
#include <dlfcn.h>
#include <pthread.h>
#include <link.h>
#include "KrellInstitute/Services/monlibs.h"


/* TODO: allow configuration to fallback to sscanf method. */
#define CBTF_USE_STROULL_FOR_MAPPINGS 1
/* hmm. maybe control this at build time? */
/* FIXME: While we do set this here it is negated later to force
 * using /proc/self/maps methods.
 */
#define CBTF_USE_DL_ITERATE 1

extern void cbtf_offline_record_dso(const char* dsoname, uint64_t begin, uint64_t end, uint8_t is_dlopen);
extern void cbtf_offline_record_dlopen(const char* dsoname, uint64_t begin, uint64_t end, uint64_t b_time, uint64_t e_time);

extern const char* CBTF_GetExecutablePath();

#if defined(RUNTIME_PLATFORM_BGP) || defined(RUNTIME_PLATFORM_BGQ)
#include <link.h>
extern etext;
extern edata;

const char *monlibs_vdso_name = "[vdso]";
const char *monlibs_exe_name = "[exe]";
static struct dl_phdr_info *cur_list = NULL;
static unsigned int cur_list_size = 0;
static struct dl_phdr_info *new_list = NULL;
static unsigned int new_list_size = 0;
#define LIST_INITIAL_SIZE 32
static unsigned int lists_max_size = 0;

static int checked_for_static = 0;
static int is_static = 0;

#if (_LP64)
#define ElfX_auxv_t Elf64_auxv_t
#define ElfX_phdr Elf64_Phdr
#else
#define ElfX_auxv_t Elf32_auxv_t
#define ElfX_phdr Elf32_Phdr
#endif

extern char **environ;

static int exe_is_static()
{
   char **walk_environ = environ;
   ElfX_auxv_t *auxv;
   ElfX_phdr *phdr = NULL;

   /* AuxV vector comes after environment */
   while (*walk_environ) walk_environ++;
   walk_environ++;

   /* Extract phdrs from auxv */
   for (auxv = (Elf32_auxv_t *) walk_environ; auxv->a_type != AT_NULL; auxv++) {
      if (auxv->a_type == AT_PHDR) {
         phdr = (ElfX_phdr *) auxv->a_un.a_val;
      }
   }

   if (!phdr) {
      fprintf(stderr, "Error - expected to find program headers\n");
   }

   /* If phdrs have a INTERP, then dynamic */
   for (; phdr->p_type != PT_NULL; phdr++) {
      if (phdr->p_type == PT_INTERP)
         return 0;
   }
   return 1;
}
#endif

#if defined(CBTF_USE_DL_ITERATE) && defined(JUNK)
static int dl_cb(struct dl_phdr_info *info, size_t size, void *data)
{
   if (new_list_size >= lists_max_size) {
      if (lists_max_size)
         lists_max_size *= 2;
      else
         lists_max_size = LIST_INITIAL_SIZE;
      cur_list = (struct dl_phdr_info *) realloc(cur_list, sizeof(struct dl_phdr_info) * lists_max_size);
      new_list = (struct dl_phdr_info *) realloc(new_list, sizeof(struct dl_phdr_info) * lists_max_size);
   }

   memcpy(new_list + new_list_size, info, sizeof(struct dl_phdr_info));
   new_list_size++;
   return 0;
}

static int dl_phdr_cmp(const void *a, const void *b)
{
   struct dl_phdr_info *first = (struct dl_phdr_info *) a;
   struct dl_phdr_info *second = (struct dl_phdr_info *) b;
   return first->dlpi_phdr > second->dlpi_phdr;
}

static void report(struct dl_phdr_info *pinfo, int is_load, library_cb cb,
		   uint64_t b_time, uint64_t e_time)
{
   mem_region mr[MAX_LOAD_SEGMENTS];
   unsigned int i, j = 0;
   ElfW(Addr) base_addr = pinfo->dlpi_addr;
   const char *name = NULL;

   if (is_load)
   {
      for (i = 0; i < pinfo->dlpi_phnum; i++) {

         const ElfW(Phdr) *phdr = pinfo->dlpi_phdr + i;
         if (phdr->p_type != PT_LOAD)
            continue;

         mr[j].mem_addr = base_addr + phdr->p_vaddr;
         mr[j].mem_size = phdr->p_memsz;
         mr[j].file_offset = phdr->p_offset;
         mr[j].file_size = phdr->p_filesz;
         mr[j].premissions = (phdr->p_flags & 7);
         j++;
      }

      if (pinfo->dlpi_name[0] == '\0') {
         /* Both the executable and vdso names are given as empty strings.  Fill in with constant strings */
         if (j == 1 && mr[0].mem_size <= 0x1000) {
            /* One segment less than a single page in size implies VDSO on linux */
            name = monlibs_vdso_name;
         }
         else {
            name = CBTF_GetExecutablePath();
         }
      }
      else {
         name = strdup(pinfo->dlpi_name);
      }

      pinfo->dlpi_name = name;
   }
   else {
      name = pinfo->dlpi_name;
   }

   cb(base_addr, name, mr, j, is_load, b_time, e_time);

   if (!is_load) {
      if (name != monlibs_vdso_name && name != monlibs_exe_name) {
         free((void *) name);
      }
   }
}

void monlibs_getLibraries(library_cb cb, uint64_t b_time, uint64_t e_time)
{
   unsigned c = 0, n = 0;
   struct dl_phdr_info *temp;

   /* Fill in the new list with the current libraries. */
   new_list_size = 0;
   dl_iterate_phdr(dl_cb, NULL);

   /* Sort the new list for fast comparison */
   qsort(new_list, new_list_size, sizeof(struct dl_phdr_info), dl_phdr_cmp);

   /* Look for differences between orig and new list */
   for (;;) {
      if (c == cur_list_size && n == new_list_size) {
         /* End of both lists */
         break;
      }
      else if (c == cur_list_size) {
         /* End of cur list, but still have new list.   New object. */
         report(new_list + n++, 1, cb, b_time, e_time);
      }
      else if (n == new_list_size) {
         /* End of new list, but still have cur list.  Deleted object */
         report(cur_list + c++, 0, cb, b_time, e_time);
      }
      else if (cur_list[c].dlpi_phdr == new_list[n].dlpi_phdr) {
         /* Same element in old and new list.  No change here. */
         new_list[n++].dlpi_name = cur_list[c++].dlpi_name;
      }
      else if (cur_list[c].dlpi_phdr < new_list[n].dlpi_phdr) {
         /* There's an element in the cur list that we didn't see in the new list.  Deleted object */
         report(cur_list + c++, 0, cb, b_time, e_time);
      }
      else if (cur_list[c].dlpi_phdr > new_list[n].dlpi_phdr) {
         /* There's an element in the new list that we didn't see in the cur list.  New object */
         report(new_list + n++, 1, cb, b_time, e_time);
      }
   }

   /* Swap the new list with the cur list.  Empty (but leave allocated) the new list */
   temp = cur_list;
   cur_list = new_list;
   new_list = temp;
   cur_list_size = new_list_size;
   new_list_size = 0;
}

static void lc(ElfW(Addr) base_address, const char *name, mem_region *regions,
		unsigned int num_regions, int is_load,
		uint64_t b_time, uint64_t e_time)

{
#ifndef NDEBUG
    if ( (getenv("CBTF_DEBUG_COLLECTOR_DSOS") != NULL)) {
	fprintf(stderr, "CBTF_GetDLInfo: %s %s at [%#lx, %#lx]\n",
		is_load ? "Loaded" : "Unloaded", name,
		regions[0].mem_addr,
		regions[0].mem_addr + regions[0].mem_size);
   }
#endif
   
    if (is_load) {
	cbtf_offline_record_dlopen(name, regions[0].mem_addr,
			      regions[0].mem_addr + regions[0].mem_size,
			      b_time, e_time);
    } else {
	cbtf_offline_record_dso(name, regions[0].mem_addr, regions[0].mem_addr + regions[0].mem_size, is_load);
    }
}
#endif

/*
 * Get the address map of executable and dsos loaded. Default to processing
 * /proc/self/maps. Can optionaly build to use linkmap info via defining
 * CBTF_USE_DL_ITERATE at build time.  The CBTF_USE_DL_ITERATE method may not
 * be safe to use within a signalhandler so care would be needed to call
 * that mathod with collection paused.
 */
int CBTF_GetDLInfo(pid_t pid, char *path, uint64_t b_time, uint64_t e_time)
{
#if defined(RUNTIME_PLATFORM_BGP) || defined(RUNTIME_PLATFORM_BGQ)
    if (checked_for_static == 0) {
	is_static = exe_is_static();
	checked_for_static = 1;
    }
    if (is_static) {
	cbtf_offline_record_dso(CBTF_GetExecutablePath(),(uint64_t)0x01000000,(uint64_t)&etext,0);
    } else {
	monlibs_getLibraries(lc,b_time,e_time);
    }
#else

    /* Process address range and mapped filename using dl_iterate methods. */
#if defined(CBTF_USE_DL_ITERATE) && defined(JUNK)
    monlibs_getLibraries(lc,b_time,e_time);
#else

    /* Process /proc/self/maps for address range and mapped filename. */
    char mapfile_name[PATH_MAX];
    FILE *mapfile;

    sprintf(mapfile_name, "/proc/%ld/maps", (long)pid);
    mapfile = fopen(mapfile_name, "r");

    if(!mapfile) {
	fprintf(stderr,"Error opening%s: %s\n", mapfile_name, strerror(errno));
	return(1);
    }

    while(!feof(mapfile)) {
	char buf[PATH_MAX+100], perm[5], dev[6], mappedpath[PATH_MAX];
	unsigned long long begin, end;

	/* read in one line from the /proc maps file for this pid. */
	if(fgets(buf, sizeof(buf), mapfile) == 0) {
	    break;
	}

	/* skip any entry that does not have 'x' permissions. */
	char *permstring = strchr(buf, (int) ' ');
	if (!(*(permstring+3) == 'x' && strchr(buf, (int) '/'))) {
	    continue;
	}

#if 0
	if ( (getenv("CBTF_DEBUG_COLLECTOR_DSOS") != NULL)) {
	    fprintf(stderr,"CBTF_GetDLInfo examine maps line: %s",buf);
	}
#endif

	/* process line with 'x' permissions. */
	/* Read in the /proc/<pid>/maps file as it is formatted. */
	/* All fields are strings. The fields are as follows. */
	/* addressrange  perms offset  dev  inode  pathname */
	/* The addressrange field is begin-end in hex. */
	/* We record these later as uint64_t. */
	/* We record the mappedpath as is and ignore the rest of the fields. */
	/* example maps line for the executable: */
	/* 20000000-20003000 r-xp 00000000 e7a:390e 1 /home/foo/bar.exe */

#if defined(CBTF_USE_STROULL_FOR_MAPPINGS)
	/* process mapping with strtoull for begin and end addresses. */
	char *end_addr, *mpath;
	/* Find the end address from buf. buf will now contain*/
	end_addr = strchr(buf, '-');
	/* Find the full path to exe or dso from buf. */
	mpath = strrchr(buf, (int) ' ');
	/* null terminate end_addr and mpath. */
	*end_addr = '\0';
	end_addr++;
	*mpath = '\0';
	mpath++;

	/* TODO: check errno on these strtoull calls if addresses
 	*  do not fit into a uint64_t.
 	*/
	begin = strtoull(buf, NULL, 16);
	end = strtoull(end_addr, NULL, 16);
	sscanf(mpath, "%s", mappedpath);

#ifndef NDEBUG
	if ( (getenv("CBTF_DEBUG_DSO_DETAILS") != NULL)) {
	    fprintf(stderr,"CBTF_GetDLInfo resolves %s with begin:%08Lx end:%08Lx\n",mappedpath,begin,end);
	}
#endif

#else   /* sscanf method for whole maps line. */
	char perm[5], dev[6];
	unsigned long long offset;
	uint64_t inode;

        int sval = sscanf(buf, "%Lx-%Lx %s %Lx %s %ld %s", &begin, &end, perm,
                &offset, dev, &inode, mappedpath);

#ifndef NDEBUG
	if ( (getenv("CBTF_DEBUG_DSO_DETAILS") != NULL)) {
	    fprintf(stderr,"CBTF_GetDLInfo sval from sscanf:%d begin:%Lx end:%Lx\n",sval,begin,end);
	}
#endif

#endif

	/* Now we record the address range and mappedpath. */
	/* If a dso is passed in the path argument we only want to record */
	/* this particular dso into the openss-raw file. This happens when */
	/* the victim application has performed a dlopen. */
	if (path != NULL &&
	    mappedpath != NULL &&
	    (strncmp(basename(path), basename(mappedpath), strlen(basename(path))) == 0) ) {
#ifndef NDEBUG
	    if ( (getenv("CBTF_DEBUG_COLLECTOR_DSOS") != NULL)) {
		fprintf(stderr,"CBTF_GetDLInfo DLOPEN RECORD: %s [%08Lx, %08Lx]\n",
		    mappedpath, begin, end);
	    }
#endif
	    cbtf_offline_record_dlopen(mappedpath, begin, end, b_time, e_time);
	    break;
	}

	/* Record the address range and mappedfile for non dlopened objects. */
	else if (path == NULL) {
#ifndef NDEBUG
	    if ( (getenv("CBTF_DEBUG_COLLECTOR_DSOS") != NULL)) {
		fprintf(stderr,"CBTF_GetDLInfo LD RECORD %s [%08Lx, %08Lx]\n", mappedpath, begin, end);
	    }
#endif
	    cbtf_offline_record_dso(mappedpath, begin, end, 0);
	}
    } /* end while feof mapfile */
    fclose(mapfile);

#endif  /* ifdef CBTF_USE_DL_ITERATE */

#endif  /* ifdef BGP or BGQ */

    return(0);
}
