import matplotlib.pyplot as plt
import matplotlib.image as mpimg
import pandas as pd
import sys
df=pd.read_csv(sys.argv[1])
ax = plt.gca()
#df.plot(kind='line',x='frame',y='dynamic density', color = 'blue' , ax=ax)
df.plot(kind='line',x='frame',y='queue density', color = 'red' , ax=ax)
plt.show()