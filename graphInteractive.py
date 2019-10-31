import sys
sys.path.append('C:\\Users\\konst\\AppData\\Local\\Programs\\Python\\Python37\\Lib\\site-packages')
import pandas as pd 
import matplotlib.pyplot as plt
#%matplotlib inline

Data = pd.read_csv('C:\\Users\\konst\\OneDrive\\Documents\\CS\\Visual Studio 2017\\Projects\\C++\\Coursework\\Coursework_console_only\\movesData.csv') # specify path

plt.plot(Data['Episodes'],Data['Q-learning'],'r',linewidth=0.5) #lineplot
plt.plot(Data['Episodes'],Data['Sarsa'],'b',alpha=0.3) #lineplot

plt.title('Algorithms Convergence')
plt.xlabel('Episodes')
plt.ylabel('Number of Moves')
plt.grid(True)
plt.legend()
plt.show()
