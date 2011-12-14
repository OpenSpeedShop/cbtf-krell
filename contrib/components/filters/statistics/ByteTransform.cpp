/*
 * Small set of templates that attempt to convert
 * data to an array of unsigned chars (bytes)
 * and back again. This is meant to be used in the
 * statistics plugin, but can probably be used
 * elsewhere.
 */

/* Take the type, T and convert it to an array of
   bytes.
 */
template <typename T>
unsigned char * ByteTransform<T>::convert (const T value) {
	T temp = value;
	unsigned char * array = (unsigned char *) calloc(sizeof(unsigned char), sizeof(T));
	for (int i = 0; i < sizeof(T); i++) {
		array[i] = ((unsigned char *) &temp) [i];
	}

	return array;
}

/* Take an array of bytes and convert it to the
   type T.  This is strictly GIGO.
 */
template <typename T>
T ByteTransform<T>::revert(unsigned char * array) {
	T value;
	for (int i = 0; i < sizeof(T); i++) {
		((unsigned char *) &value)[i] = array[i];
	}

	return value;
}

/* This should tell us how large the array of bytes
   is for the type T.
 */
template <typename T>
int ByteTransform<T>::getSize() {
	return sizeof(T);
}
