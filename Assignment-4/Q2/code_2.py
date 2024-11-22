import csv
import matplotlib.pyplot as plt
import pandas as pd

file = open("q2.tr",'r')
filtered_lines=[]
for line in file:
    filter=line.split()
    if(filter[0]=='+' or filter[0]=='-'):
        node_name=filter[2].split('/')
        if(node_name[2]=='0'):
            filtered_lines.append(line)


record={}
for line in filtered_lines:
    data=line.split()
    pkt_id=data[18]
    time=data[1]
    operation=data[0]
    if pkt_id in record:
        if(operation=='+'):
            record[pkt_id][0]=time
        else:
            record[pkt_id][1]=time
    else:
        if(operation=='+'):
            record[pkt_id]=[time,0]
        else:
            record[pkt_id]=[0,time]

output_file = open('q2_delay.csv','w',newline='')
writer=csv.writer(output_file)

for key in record:
    data=record[key]
    new_data_sample=[0,0] #[x,y] [time,wait]
    diff=float(data[1])-float(data[0])
    new_data_sample[0]=float(data[1])
    new_data_sample[1]=diff
    writer.writerow(new_data_sample)

output_file.close()
file.close()




file_path = "q2_delay.csv"  
data = pd.read_csv(file_path,header=None)

data = data[(data.iloc[:, 1] >= 0) & (data.iloc[:, 0] >= 0)]  

x = data.iloc[:, 0]  
y = data.iloc[:, 1]  

plt.figure(figsize=(8, 6))
plt.plot(x, y, linestyle='-', color='b')  
plt.xlabel("Time")
plt.ylabel("Delay")
plt.title("Delay vs Time")
plt.grid(True)

output_file = "q2_delay_img.png"  
plt.savefig(output_file, dpi=300)

plt.show()