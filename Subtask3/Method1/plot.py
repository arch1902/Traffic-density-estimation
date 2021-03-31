import matplotlib.pyplot as plt
import matplotlib.image as mpimg
import pandas as pd
import sys
df=pd.read_csv(sys.argv[1])
ax = plt.gca()
#df.plot(kind='line',x='frame',y='dynamic density', color = 'blue' , ax=ax)
# plt.title("Dynamic Density Using Sparse OF")
plt.xlabel("x")
plt.ylabel("Mean Sqaured Error(10^-5)")
df.plot(kind='line',x='x',y='error', color = 'red' , ax=ax)
plt.show()