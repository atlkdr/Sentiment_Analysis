import random
import math	
import operator

#-------------------------------PRE-PROCESSING PART OF THE PROBLEM -------------------------------


def printer(writer,sample):
	for line in sample:
		for word in line:
			print>>writer,word,	
		print>>writer,'\n',		

def printer2(writer,sample):
	for attr in sample:
		print>>writer,attr


def data_selector(filename,state):
	all_data=[]
	with open(filename,'r') as t_data:
		for line in t_data:
			temp_dump=[]
			temp_dump_2=[]
			for word in line.split():
				temp_dump.append(word)
			if len(temp_dump)!=0:
				temp_dump[0]=int(temp_dump[0])
				temp_dump_2.append(temp_dump[0])
				for word in temp_dump[1:]:
					x=word.split(':')
					temp_dump_2.append(int(x[0]))
				all_data.append(temp_dump_2)
	
	pos_sample=[]
	neg_sample=[]
	chossen_index_array=[]

	while(len(pos_sample)<500):
		index=random.randint(0,(len(all_data)-1))
		if index not in chossen_index_array:
			if((all_data[index][0])>=7):
				chossen_index_array.append(index)
				pos_sample.append(all_data[index])
	
	while(len(neg_sample)<500):
		index=random.randint(0,(len(all_data)-1))
		if index not in chossen_index_array:
			if((all_data[index][0])<=4):
				chossen_index_array.append(index)
				neg_sample.append(all_data[index])

	if state==0:
		writer=open('Training_Data_Selected','w')
	else:
		writer=open('Test_Data_Selected','w') 
	
	printer(writer,pos_sample)
	printer(writer,neg_sample)	
	
	writer.close()

	return pos_sample,neg_sample

def att_collector(filename):
	# 1 st word at 0th index
	all_attr={}
	index=0
	with open(filename,'r') as attr:
		for line in attr:
			for word in line.split():
				all_attr[float(word)]=index
				index+=1	
	all_attr_1=sorted(all_attr.items(),key=operator.itemgetter(0))
	all_attr_2=sorted(all_attr.items(),key=operator.itemgetter(0),reverse=True)
	chosen_attr_1=[]
	chosen_attr_2=[]
	chosen_attr=[]
	limit=2500;
	
	for i in all_attr_1:
		if(limit<=0):
			break
		limit=limit-1
		chosen_attr_1.append(i[1])
	
	limit=2500;

	for i in all_attr_2:
		if(limit<=0):
			break
		limit=limit-1
		chosen_attr_2.append(i[1])	
	for i in range(len(chosen_attr_1)):
		chosen_attr.append(chosen_attr_1[i])
		chosen_attr.append(chosen_attr_2[i])
		
	writer2=open('Chosen-Attr-Index','w')
	printer2(writer2,chosen_attr)
	return chosen_attr


#-------------------------------PRE-PROCESSING PART OF THE PROBLEM END-------------------------------

feature=att_collector('Given_Data/imdbEr.txt')
pos_train_sample,neg_train_sample=data_selector('Given_Data/Train_Data_Given/labeledBow.feat',0)
pos_test,neg_test=data_selector('Given_Data/Test_Data_Given/labeledBow.feat',1)
