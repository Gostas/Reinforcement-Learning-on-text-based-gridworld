import sys
sys.path.append('C:\\Users\\konst\\AppData\\Local\\Programs\\Python\\Python37\\Lib\\site-packages')
import pandas as pd 
import matplotlib.pyplot as plt
#%matplotlib inline

Data = pd.read_csv('C:\\Users\\konst\\OneDrive\\Documents\\CS\\Visual Studio 2017\\Projects\\C++\\Coursework\\Coursework_console_only\\movesDataBatch.csv') # specify path

plt.plot(Data['Variable'],Data['Q-Learning'],'r',linewidth=0.5) #lineplot
plt.plot(Data['Variable'],Data['Sarsa'],'b',alpha=0.3) #lineplot
plt.plot(Data['Variable'],Data['Average'],'p',alpha=0.3) #lineplot

plt.title('Batch Processing with variable gamma')
plt.xlabel('Gamma')
plt.ylabel('Number of Episodes Untill Convergence')
plt.grid(True)
plt.legend()
plt.show()
