#include<iostream>
#include<vector>
#include<string>
#include<list>
#include<fstream>
#include<math.h>
#include<bits/stdc++.h>
#include<sstream>
#include<algorithm>

using namespace std;

int attr_iteration(list<int> ll)
{
  list<int>::iterator itr;
  for(itr=ll.begin();itr!=ll.end();itr++)
  {
    cout<<*itr<<endl;
  } 
}

int list_iteration(list< list<int> > ll)
{
   list< list<int> >::iterator itr;
   for (itr=ll.begin(); itr != ll.end(); itr++)
   {
       list<int>tl=*itr;
       list<int>::iterator it;
       for (it=tl.begin(); it != tl.end(); it++)
       {
           cout<<*it<<" ";
       }
       cout<<endl;
   }
   return 0;
}


list< list<int> > readFile(string file_name)
{
    ifstream file;
    file.open(file_name.c_str(),ios::in);
    string word;
    string line;
    list< list<int> > ll;
    while(getline(file,line))
    {
      stringstream iss(line);
      list<int> line;
      while(iss>>word)
      {
        stringstream con(word);
        int temp = 0;
        con>>temp;
      line.push_back(temp);
      }
      ll.push_back(line);

    }
    return ll;
}

list<int> readAttr(string file_name)
{
    ifstream file;
    file.open(file_name.c_str(),ios::in);
    string word;
    string line;
    list<int> ll;
    while(file>>word)
    {
      stringstream con(word);
      int temp = 0;
      con>>temp;
    ll.push_back(temp);
    }
    return ll;
}

class Tnode
{
  public:
    int attr;
    int label;
    int is_leaf;
    Tnode* agree=NULL;
    Tnode* disagree=NULL; 
};


int predict(list<int> temp,Tnode* root)
{
  if(root->is_leaf==1)
  {
    return root->label;
  }
  int attribute=root->attr;
  bool present=(std::find(temp.begin(),temp.end(),attribute)!=temp.end());
  if(present)
  {
    return predict(temp,root->agree);
  }
  else
  {
    return predict(temp,root->disagree);
  }
}

double test_function(list< list<int> > data,Tnode* root,int state)
{
  list< list<int> > :: iterator i1;
  double count1=0;
  double count2=0;
  for(i1=data.begin();i1!=data.end();i1++)
  {
    list<int> temp=*i1;
    int status=predict(temp,root); // CAN Be used  to check not Found Case in test Data
    if(state==1)
      {
        if(status>=0)
        count1+=status;
      }
    else if(state==-1)
      {
        if(status<=0)
        {
        	status=status*-1;
        	count2+=status;
        }
      }
  }
  if(state==1)
  return count1;
  else
  	return count2;
}

double entropy(double p,double n)
{
  if((p!=0)&&(n!=0))
    return -1*((p*log2(p))+(n*log2(n)));
  else
    return 0;
} 

double gain_func(double agree_pos,double agree_neg,double dis_pos,double dis_neg)
{
  double node_pos=((agree_pos+dis_pos)/(agree_pos+agree_neg+dis_pos+dis_neg));
  double node_neg=1-node_pos;
  double node_entropy=entropy(node_pos,node_neg);

  double agg_pos_ratio=((agree_pos)/(agree_pos+agree_neg));
  double agg_neg_ratio=1-agg_pos_ratio;
  double agg_node_entropy=entropy(agg_pos_ratio,agg_neg_ratio);

  double dis_pos_ratio=((dis_pos)/(dis_pos+dis_neg));
  double dis_neg_ratio=1-dis_pos_ratio;
  double dis_node_entropy=entropy(dis_pos_ratio,dis_neg_ratio);

  double agg_ratio=((agree_pos+agree_neg)/(agree_pos+agree_neg+dis_pos+dis_neg));
  double dis_ratio=1-agg_ratio;
  double gain=node_entropy-((agg_ratio*agg_node_entropy)+(dis_ratio)*dis_node_entropy);

  return gain;
}

int best_split(list< list<int> > pos_sen,list< list<int> > neg_sen,list<int> attr)
{
  list<int> :: iterator it1;
  list< list<int> > :: iterator it2;

  double max_gain=0.0;
  int best=-99;
  int Current_max=1000;                    // Choosing At Max Attributes out of 5000 Selected 
  for(it1=attr.begin();it1!=attr.end();it1++)
  {

    //Time Limiter
    if(Current_max<=0)
      break;
    Current_max-=1;

    double gain=0.0;
    int atri=*it1;

    double agree_pos=0.0;
    double agree_neg=0.0;
    double dis_pos=0.0;
    double dis_neg=0.0;
    
    for(it2=pos_sen.begin();it2!=pos_sen.end();it2++)
    {
      list<int> temp=*it2;
      bool found=(std::find(temp.begin(),temp.end(),atri)!=temp.end());
      if(found)
      {
        agree_pos=agree_pos+1;
      }
      else{
        dis_pos=dis_pos+1;
      }
    }
    for(it2=neg_sen.begin();it2!=neg_sen.end();it2++)
    {
      list<int> temp=*it2;
      bool found=(std::find(temp.begin(),temp.end(),atri)!=temp.end());
      if(found)
      {
        agree_neg=agree_neg+1;
      }
      else{
        dis_neg=dis_neg+1;
      }
    }
    gain=gain_func(agree_pos,agree_neg,dis_pos,dis_neg);
    if(gain>max_gain)
    {
      max_gain=gain;
      best=atri;
    }
  }
  if(max_gain>0.0)
  {
    return best;
  }
  else
  {
    return -99;
  }
}





Tnode* Tree(list< list<int> > pos_sen,list< list<int> > neg_sen,list<int> features,int depth,int &nodes,int &leaf_node)
{
  nodes=nodes+1;
  Tnode* node=new Tnode();
  int pos_size=pos_sen.size();
  int neg_size=neg_sen.size();

  if(features.size()==0)
  {
    node->is_leaf=1;
    node->attr=-99;
    if((pos_size-neg_size)>0)
    {
      node->label=1;
    }
    else if((pos_size-neg_size)<0)
    {
      node->label=-1;
    }
    else
    {
      node->label=0;
    }
    leaf_node=leaf_node+1;
    return node;
  }
  if(depth<=0)
  {
    node->is_leaf=1;
    node->attr=-99;
    if((pos_size-neg_size)>0)
    {
      node->label=1;
    }
    else if((pos_size-neg_size)<0)
    {
      node->label=-1;
    }
    else
    {
      node->label=0;
    }
    leaf_node=leaf_node+1;
    return node;
  }
  if(pos_size<=0)
  {
    if(neg_size>0)
    {
      node->label=-1;
      node->is_leaf=1;
      node->attr=-99;
      leaf_node=leaf_node+1;
      return node;
    }
    else
    {
      node->label=0;
      node->is_leaf=1;
      node->attr=-99;
      leaf_node=leaf_node+1;
      return node;
    }
  }
  if(neg_size<=0)
  {
    if(pos_size>0)
    {
      node->label=1;
      node->is_leaf=1;
      node->attr=-99;
      leaf_node=leaf_node+1;
      return node;
    }
    else
    {
      node->is_leaf=1;
      node->label=0;
      node->attr=-99;
      leaf_node=leaf_node+1;
      return node;
    }
  }
  int split_attr=best_split(pos_sen,neg_sen,features);
  if(split_attr==-99)
  {
    node->is_leaf=1;
    node->attr=-99;
    leaf_node=leaf_node+1;
    if((pos_size-neg_size)>0)
    {
      node->label=1;
    }
    else
    {
      node->label=-1;
    }
    return node;
  }
  //cout<<"Attribute Found: "<<split_attr<<endl;

  node->attr=split_attr;
  
  if((pos_size-neg_size)>0)
    node->label=1;
  else
    node->label=-1;

  node->is_leaf=0;

  //cout<<"NOde LaBEL "<<node->label<<endl;

  list< list<int> > agree_pos_sen,agree_neg_sen,dis_pos_sen,dis_neg_sen;
  list< list<int> > :: iterator itr;
  for(itr=pos_sen.begin();itr!=pos_sen.end();itr++)
  {
    list<int> temp=*itr;
    bool found=(std::find(temp.begin(),temp.end(),split_attr)!=temp.end());
      if(found)
      {
        agree_pos_sen.push_back(temp);
      }
      else{
        dis_pos_sen.push_back(temp);
      } 
  }
  for(itr=neg_sen.begin();itr!=neg_sen.end();itr++)
  {
    list<int> temp=*itr;
    bool found=(std::find(temp.begin(),temp.end(),split_attr)!=temp.end());
      if(found)
      {
        agree_neg_sen.push_back(temp);
      }
      else{
        dis_neg_sen.push_back(temp);
      } 
  }

  // Erase Split Attribute From Features

  // cout<<"Sizes: "<<neg_sen.size()<<" "<<pos_sen.size()<<endl;

  bool found_feature=(std::find(features.begin(),features.end(),split_attr)!=features.end());
  if(found_feature)
  {
    features.remove(split_attr);
  }
  else
  {
    cout<<"INVALID ATTRIBUTE ERROR Line 252"<<endl;
  }

  if((agree_pos_sen.size()==0)&&(agree_neg_sen.size()==0))
  {
    // The Agree Subnode Is Empty

    Tnode* temp_node=new Tnode();
    temp_node->is_leaf=1;
    temp_node->label=0;
    temp_node->attr=-99;   // Default Invalid Value
    node->agree=temp_node;
    leaf_node=leaf_node+1;

  }
  else{
    node->agree=Tree(agree_pos_sen,agree_neg_sen,features,depth-1,nodes,leaf_node);
  }

  if((dis_pos_sen.size()==0)&&(dis_neg_sen.size()==0))
  {
    Tnode* temp_node=new Tnode();
    temp_node->is_leaf=1;
    temp_node->label=0;
    temp_node->attr=-99;
    node->disagree=temp_node;
    leaf_node=leaf_node+1;

  }
  else{
    node->disagree=Tree(dis_pos_sen,dis_neg_sen,features,depth-1,nodes,leaf_node);
  }
  return node;
}

void Traversal(Tnode* p,int indent)
{
  if(p != NULL) {
        if(p->disagree)
          Traversal(p->disagree,indent+4);
        if(p->agree) 
          Traversal(p->agree,indent+4);
        if (indent) {
            cout<<std::setw(indent)<<"--";
        }
        cout<<"Label "<<p->label<<" Attribute "<<p->attr<<endl;
    }
}

void Stopping_Early(list< list<int> > pos,list< list<int> > neg,list<int> features,list< list<int> > pos_test,list< list<int> > neg_test)
{
	for(int i=4;i<=10;i++)
	{
		int depth=i;
		int nodes=0;
		int leaf_node=0;
		Tnode* node=Tree(pos,neg,features,depth,nodes,leaf_node);
		double pos_accur=test_function(pos,node,1);
		double neg_accur=test_function(neg,node,-1);
		pos_accur=pos_accur/pos.size();
		neg_accur=neg_accur/neg.size();
		pos_accur=pos_accur*100;
		neg_accur=neg_accur*100;
    double total_accuracy_train=(pos_accur+neg_accur)/2;

    //Reuse Variables
    pos_accur=test_function(pos_test,node,1);
    neg_accur=test_function(neg_test,node,-1);
    pos_accur=pos_accur/pos_test.size();
    neg_accur=neg_accur/neg_test.size();
    pos_accur=pos_accur*100;
    neg_accur=neg_accur*100;
    double total_accuracy_test=(pos_accur+neg_accur)/2;

		cout<<"Depth-of-Tree "<<depth<<" Accuracy[Train]: "<<total_accuracy_train<<" Nodes: "<<nodes<<" Accuracy[Test]: "<<total_accuracy_test<<endl;	
	}
	return;
}

void Noise_addition(list< list<int> > pos,list< list<int> > neg,list<int> features)
{
	double noise_add[4]={0.005,0.01,0.05,0.1};
	int length=pos.size();
	int depth=40;          // Current Depth Limit for Noise_addition 
	int nodes=0;
	int leaf_node=0;
	Tnode* no_noise_node=Tree(pos,neg,features,depth,nodes,leaf_node);
	for(int i=0;i<4;i++)
	{
		double noise=noise_add[i]*length;

		list< list<int> > pos_copy=pos;
		list< list<int> > neg_copy=neg;
		for(int k=0;k<noise;k++)
		{
      list<int> holdy1=neg_copy.front();
			pos_copy.push_back(neg_copy.front());
			neg_copy.push_back(pos_copy.front());
			neg_copy.pop_front();
			pos_copy.pop_front();
      list<int> holdy=pos_copy.back();
      //cout<<"New pos.back():"<<holdy1.front()<<"=="<<holdy.front();   // Checker
		}	
		int nodes_noise=0;
		int leaf_node_noise=0;

		Tnode* noise_node=Tree(pos_copy,neg_copy,features,depth,nodes_noise,leaf_node_noise);
		cout<<" Nodes-no-Noise:"<<nodes<<" Noise-Ratio:"<<noise_add[i]<<" Equivalent-noisy-entries:"<<noise*2<<" Noise-nodes:"<<nodes_noise<<endl; 
	} 
}


double Pruning_accuracy(Tnode* node,list< list<int> > pos,list< list<int> > neg)
{
		double pos_accur=test_function(pos,node,1);
		double neg_accur=test_function(neg,node,-1);
		pos_accur=pos_accur/pos.size();
		neg_accur=neg_accur/neg.size();
		pos_accur=pos_accur*100;
		neg_accur=neg_accur*100;
		double Total_accur=(pos_accur+neg_accur)/2;
    return Total_accur;
}


void nodes_give(Tnode* root,list<Tnode*> &nodes)
{
  if(root->agree!=NULL)
    nodes_give(root->agree,nodes);
  if(root->disagree!=NULL)
    nodes_give(root->disagree,nodes);
  nodes.push_back(root);
} 

int node_count(Tnode* root)
{
  if(root->is_leaf==1)
    return 1;
  else
    return 1+node_count(root->agree)+node_count(root->disagree);
}

void Pruner(list< list<int> > pos_sen1,list< list<int> > neg_sen1,list<int> features,int depth)
{
   int nodes=0;
   int leaf_nodes=0;
   
   Tnode* root=Tree(pos_sen1,neg_sen1,features,depth,nodes,leaf_nodes);
   int after_prun_nodes=nodes+leaf_nodes;

   list<Tnode*> node_point;
   nodes_give(root,node_point);
   //cout<<"Nodes: "<<node_point.size()<<" "<<nodes<<endl;             ///checker
   double accuracy=Pruning_accuracy(root,pos_sen1,neg_sen1);

   cout<<"Accuracy-Before-Pruning: "<<accuracy<<" Nodes:"<<after_prun_nodes<<endl;  //Old Value
   for(list<Tnode*> ::iterator itr=node_point.begin();itr!=node_point.end();itr++)
   {
      Tnode* temp=*itr;
      if(temp->is_leaf==0)
        {
          int lost_nodes=node_count(temp);
          temp->is_leaf=1;
          double new_accuracy=Pruning_accuracy(root,pos_sen1,neg_sen1); 
          cout<<"Accuracy-During-Pruning: "<<new_accuracy<<" Reduced-Nodes:"<<lost_nodes<<endl;
          if(new_accuracy<accuracy)
          temp->is_leaf=0;
          else
          {
            after_prun_nodes=after_prun_nodes-lost_nodes+1; // Balance the Miscount
            accuracy=new_accuracy;
          }     
        }
   }
   cout<<"Accuracy-After-Pruning: "<<accuracy<<" Nodes:"<<after_prun_nodes<<endl;

}


void Random_Forest(list< list<int> > pos,list< list<int> > neg,list<int> features,int depth)
{ 
	cout<<"Function In Random_Forest"<<endl;
	int no_of_trees=5;
	int no_of_forest=3;
	for(int i=0;i<no_of_forest;i++)
		{
			no_of_trees=no_of_trees+i*5;             // 5 , 10 ,15 
			double forest_accur=0;

      double count_pos_accur[pos.size()];
      double count_neg_accur[neg.size()];
      for(int m=0;m<pos.size();m++)
        count_pos_accur[m]=0;
      for(int m=0;m<neg.size();m++)
        count_neg_accur[m]=0;

			for(int k=0;k<no_of_trees;k++)
			{
				vector<int> temp(features.begin(),features.end());
				random_shuffle(temp.begin(),temp.end());
				copy(temp.begin(),temp.end(),features.begin());
				// New Set of Random Features
				
				int nodes=0;
				int leaf_node=0;
				Tnode* node=Tree(pos,neg,features,depth,nodes,leaf_node);
				int index=0;
        for(list< list<int> > :: iterator ito=pos.begin();ito!=pos.end();ito++)
        {
          list< list<int> > temp2;
          temp2.push_back(*ito);
          count_pos_accur[index]=test_function(temp2,node,1);
          index=index+1;
        } 
        index=0;
        for(list< list<int> > :: iterator ito=neg.begin();ito!=neg.end();ito++)
        {
          list< list<int> > temp2;
          temp2.push_back(*ito);
          count_neg_accur[index]=test_function(temp2,node,-1);
          index=index+1;
        } 				
			}
      double p1=0.0;
      for(int m=0;m<pos.size();m++)
        {
          if(count_pos_accur[m]>0)
            p1+=1;
        }
      p1=p1/pos.size();
      double n1=0.0;
      for(int m=0;m<neg.size();m++)
        {
          if(count_neg_accur[m]>0)
            n1+=1;
        } 
       n1=n1/neg.size();
       forest_accur=(n1+p1)/2; 
			cout<<"Tress in Forest: "<<no_of_trees<<" Forest_accuracy_on_train_data: "<<forest_accur<<endl;
		}
}

int main(int argc,char* argv[])
{

   list< list<int> > train=readFile("Training_Data_Selected");
   list< list<int> > test=readFile("Test_Data_Selected");

   list<int> features=readAttr("Chosen-Attr-Index");

   //Iterators

   list< list<int> > :: iterator itr;
   list<int> :: iterator itr2;
   
   //Positive and Negative Sentiment of Training _ data 
   list< list<int> > pos_sen1;
   list< list<int> > neg_sen1;
   
   list< list<int> > pos_sen_test;
   list< list<int> > neg_sen_test;

   for(itr=train.begin();itr!=train.end();itr++)
   {
   	list<int> temp=*itr;
   	if((*temp.begin())>=7)
   	{
   		temp.pop_front();
   		pos_sen1.push_back(temp);
   	}
   	else if((*temp.begin())<=4)
   	{
   		temp.pop_front();
   		neg_sen1.push_back(temp);
   	}
   }
   for(itr=test.begin();itr!=test.end();itr++)
   {
    list<int> temp=*itr;
    if((*temp.begin())>=7)
    {
      temp.pop_front();
      pos_sen_test.push_back(temp);
    }
    else if((*temp.begin())<=4)
    {
      temp.pop_front();
      neg_sen_test.push_back(temp);
    }
   }
   int status=1;
   while(status!=0)
   {
    cout<<"---------For Stopping_Early: Analysis:Press 1---------"<<endl;
    cout<<"---------For Pruning: Analysis:Press 2---------"<<endl;
    cout<<"---------For Noise_addition: Analysis:Press 3---------"<< endl;
    cout<<"---------For Random_forest: Analysis:Press 4---------"<<endl;
    cout<<"---------TO EXIT : PRESS 0---------"<<endl;
    cin>>status;
    cout<<"\n";

    

    if(status==1){
    Stopping_Early(pos_sen1,neg_sen1,features,pos_sen_test,neg_sen_test);
    }


    if(status==3){
    Noise_addition(pos_sen1,neg_sen1,features);
   }

   int depth=10; //Current Limited Depth


   if(status==2){
     Pruner(pos_sen1,neg_sen1,features,depth);
   }

   if(status==4){
     Random_Forest(pos_sen1,neg_sen1,features,depth);
   }

   }
  return 0; 
} 