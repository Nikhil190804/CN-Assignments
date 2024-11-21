import csv

file = open("q1.tr",'r')
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

output_file = open('q1_delay.csv','w')
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