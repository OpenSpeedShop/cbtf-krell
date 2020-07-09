#!/usr/bin/env python 
import csv
import ast
import glob
import copy


class CSV_Record:
    """Stores summary data for cbtf csv data

    Attributes:
        data_dirname (str): Top-level directory where csv data is stored
        thread_list (List[int]): List of threads from the app
        thread_cnt (int): Number of threads
        thread_data (Dict): Summary of csv data for each thread 

    """
    def __init__(self, data_dirname):
        self.data_dirname = data_dirname
        self.thread_list = []
        self.thread_cnt = 0
        self.thread_data = {}
        self.summary_data = {}
        self.__build_thread_list()
        self.__process_all_threads()


    def calculate_max_time(self):
        """Calculates the max time of the app by checking max of each thread

        Returns:
            float: Max thread time of the application
    
        """
        max_times = [t[0]['SUMMARY'][0]['total_time_seconds'] for t in self.thread_data.values()]
        return max(max_times)


    def __build_thread_list(self):
        """Compute a thread_list used to generate metrics per thread

        Some glob example patterns.  glob.iglob uses iterators.
        All csv dirs. 1 per rank/process:
           all_csv_dirs = glob.glob(data_dirname + '/*')
        All csv files:
           all_csv_files = glob.glob(data_dirname + '/*/*.csv')
        Thread 0 (main thread) csv files:
           main_thread_files = glob.glob(data_dirname + '/*/*-0.csv')
        Thread specific csv files (search on simple tid int value):
           search_pattern = '/*/*-' + str(thread_id) + '.csv'
           tid_files = glob.glob(data_dirname + search_pattern)
        

        """

        csv_file_cnt = len(glob.glob(self.data_dirname + '/*/*.csv'))
        thread_file_cnt = -1
        file_cnt = 0
        while True :
           s = '/*/*-' + str(self.thread_cnt) + '.csv'
           files = glob.glob(self.data_dirname + s)
           thread_file_cnt = len(files)
           if thread_file_cnt > 0:
                file_cnt += thread_file_cnt
                self.thread_list.append(self.thread_cnt)
           if file_cnt == csv_file_cnt:
              break
           self.thread_cnt += 1


    def __process_all_threads(self):
        """Process all threads and ranks in the csv directory

        """
        for t in self.thread_list:
            self.thread_data[t] = self.__process_csv_directory(t)

 
    def __process_csv_directory(self, t):
        max_data = None
        min_data = None
        sum_data = None
        count = 0
        search_pattern = '/*/*-' + str(t) + '.csv'
        thread_files = glob.glob(self.data_dirname + search_pattern)
        for f in thread_files:
            data = self.__process_csv_file(f)
            count += 1
            #print ("FILE:" + str(f) + " count:" + str(count))
            #print ("DATA:" + str(data))
            if not max_data:
                # copy initial data dict from first file.
                max_data = copy.deepcopy(data)
                min_data = copy.deepcopy(data)
                sum_data = copy.deepcopy(data)
                continue
    
            # there may be cases where a thread has gathered data
            # that may not be found in other threads. These tables
            # represent activity across all items in a particular
            # thread across all ranks.
            for key, value in data.iteritems():
                if not key in max_data:
                    max_data[key] = num(value)
                if not key in min_data:
                    min_data[key] = num(value)
                if not key in sum_data:
                    sum_data[key] = num(value)
    
            # handle sum of incoming values. later used to compute average
            # values based on rank and/or thread counts.
            for i,isum, in zip(data.iteritems(),sum_data.iteritems()):
                # use incoming data to index subitem data for sum  by item lookup.
                sumitem = sum_data.get(i[0])
                dataitem = data.get(i[0])
                for r,rsum in zip(dataitem,sumitem):
                    for irsum in rsum.iteritems():
                        # iterate on the current entries in sum_data.
                        # if the incoming csv does not have this item
                        # then we skip updating the sum.
                        val = r.get(irsum[0])
                        if val != None:
                            rsum.update({ irsum[0]: r.get(irsum[0]) + rsum.get(irsum[0]) })
    
            # handle max and min recording of incoming values.
            for i,imin,imax, in zip(data.iteritems(),min_data.iteritems(),max_data.iteritems()):
                # use incoming data to index sub data for max and min  by item lookup.
                maxitem = max_data.get(i[0])
                minitem = min_data.get(i[0])
                dataitem = data.get(i[0])
                for r,rmin,rmax in zip(dataitem,minitem,maxitem):
                    for ir in r.iteritems():
                        rmax.update({ ir[0]: max(r.get(ir[0]), rmax.get(ir[0])) })
                        rmin.update({ ir[0]: min(r.get(ir[0]), rmin.get(ir[0])) })
        return [max_data, min_data, sum_data, count]


    def __parse_str(self, s):
        try:
    	# maybe convert int to long here?
            return ast.literal_eval(str(s))
        except:
            return s
   
 
    def __process_csv_file(self, filename):
        category = ""
        cat_num = 0
        header = None
        toggle = None
        record_data = {}
        table = []
        def make_record():
            return [dict(zip(header, x)) for x in table]
        with open(filename, 'rU') as csv_data:
            reader = csv.reader(csv_data)
            for row in reader:
                if not toggle:
                    header = tuple(x.strip() for x in row)
                    # categorize the incoming data.  This is dependent
                    # on the format of the incoming csv. In particular we
                    # are sensitive to the fields used here to create the
                    # categories being maintained in the csv schema.
                    for s in header:
                        if 'executable' in s:
                            category = "SUMMARY"
                            break
                        if 'utime_seconds' in s:
                            category = "RUSAGE"
                            break
                        if 'dmem_high_water_mark_kB' in s:
                            category = "DMEM"
                            break
                        elif 'io_total_time' in s:
                            category = "IO"
                            break
                        elif 'total_mpi_time_seconds' in s:
                            category = "MPI"
                            break
                        elif 'idle_time' in s:
                            category = "OMPT"
                            break
                        elif 'allocation_calls' in s:
                            category = "MEMALLOC"
                            break
                        elif 'free_calls' in s:
                            category = "MEMFREE"
                            break
                        elif 'PAPI' in s:
                            category = "PAPI"
                            break
                        else:
                            # unknown data. Should always check this.
                            category = "DATA" + str(cat_num)
                    table = []
                    cat_num = cat_num+1
                    toggle = True
                else:
                    table.append([self.__parse_str(x.strip()) for x in row])
                    if category == 'SUMMARY':
                        for i, item in enumerate(header):
                            if header[i] in self.summary_data:
                                self.summary_data[header[i]].append(row[i])
                            else:
                                self.summary_data[header[i]] = [row[i]]
                    toggle = None
                if not toggle:
                    record_data[category] = make_record()
        return record_data
