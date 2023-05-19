
import numpy as np
import matplotlib.pyplot as plt
import matplotlib.tri as tri
import numpy as np
a=np.genfromtxt(".\Cell0Ds.csv");
b=np.genfromtxt(".\Cell2Ds.csv",skip_header=1);
x=a[1:,2]
y=a[1:,3]
triangles=b[:,1:4]
triang=tri.Triangulation(x,y,triangles)
plt.triplot(triang)
plt.savefig("NewMesh.png")
plt.close()
