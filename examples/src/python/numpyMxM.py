#
# Example code from url:
# jameshensman.wordpress.com/2010/06/14/multiple-matrix-multiplication-in-numpy
#
# The following runs a quick test, multiplying 1000 3 by 3 matrices together.
# It is a little crude, but it shows the numpy array method to be 10 times
# faster than the list comp of np matrix.
#
import numpy as np
import timeit
 
#compare multiple matrix multiplication using list coms of matrices and deep arrays
 
#1) the matrix method
setup1 = """
import numpy as np
A = [np.mat(np.random.randn(3,3)) for i in range(1000)]
B = [np.mat(np.random.randn(3,3)) for i in range(1000)]
"""
 
test1 = """
AB = [a*b for a,b in zip(A,B)]
"""
 
timer1 = timeit.Timer(test1,setup1)
print "Time for matrix method:"
print timer1.timeit(100)
 
#2) the array method
setup2 = """
import numpy as np
A = np.random.randn(1000,3,3)
B = np.random.randn(1000,3,3)
"""
 
test2 = """
AB = np.sum(np.transpose(A,(0,2,1)).reshape(1000,3,3,1)*B.reshape(1000,3,1,3),0)
"""
 
timer2 = timeit.Timer(test2,setup2)
print "Time for array method:"
print timer2.timeit(100)
