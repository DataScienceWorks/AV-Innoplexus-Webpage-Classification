#include<stdio.h>
#include<iostream>
#include<vector>
#include<sstream>
#include<fstream>
#include<string>
#include<stdlib.h>
#include <cstring>
#include<algorithm> // for sort()
#include <cctype>
#define TRAINING 1
#define TESTING 0
using namespace std;
void ReadFile(char *trainfilepath,vector<vector<string> >&TrainingTok,int &max_num_tokens){
	std::ifstream fileptr(trainfilepath);
	string line;
	bool rm_first = false;
	if (fileptr.is_open()){
		while (getline (fileptr, line))
		{
			if(!rm_first){
				rm_first = true;
				continue;
			}
			istringstream ss(line);
			string item;
			vector<string> tokens;
			tokens.push_back("-1");
			int not_req=0;
			while (getline(ss, item, ',')) {
				if(item[0]=='"'){
					item.erase(0,1);
				}
				if(item[strlen(item.c_str())-1]=='"'){
					item.erase(strlen(item.c_str())-1,strlen(item.c_str()));
				}

				if(not_req!=1){
					if(not_req==2){
						istringstream ss2(item);
						string item2;
						while(std::getline(ss2, item2))
						{
							vector<string>wordVector;
							std::size_t prev = 0, pos;
							while ((pos = item2.find_first_of(" ';/?=&-.%_", prev)) != std::string::npos)
							{
								if (pos > prev)
									wordVector.push_back(item2.substr(prev, pos-prev));
								prev = pos+1;
							}
							if (prev < item2.length())
								wordVector.push_back(item2.substr(prev, std::string::npos));
							for(int w=0;w<wordVector.size();w++){
								if(w>0){

									bool is_ins = true;
									if((wordVector[w]=="www")||(wordVector[w]=="org")||(wordVector[w]=="com")){
										is_ins = false;
									}
									if(is_ins){
										tokens.push_back(wordVector[w]);
									}
								}
							}

						}
						/*                                            int cnt=0;
                            				while(getline(ss2,item2,'/')){

                            					if(cnt>1){
                            						tokens.push_back(item2);
                            					}
                            					cnt++;
                            				}*/

					}else{

						tokens.push_back(item);
					}
				}
				not_req++;
			}
			if(max_num_tokens<tokens.size()-1){
				max_num_tokens=tokens.size();
			}
			/*for(int t1=0;t1<tokens.size();t1++){
                            	cout<<tokens[t1]<<" ";
                            }
                            cout<<endl;*/
			//if(tokens.size()>2){
			TrainingTok.push_back(tokens);
			//}
		}
	}
}
void MappedTokens(vector<vector<string> >&TrainingTok,vector<vector<string> >&MappedTok,vector<vector<string> >&FeaTok,int ite,bool &is_hit){
	for(int i=0;i<TrainingTok.size();i++){
		if((TrainingTok[i][0]=="-1")&&(TrainingTok[i].size()>ite)){
			is_hit = true;
			vector<string>temp;
			for(int t=2;t<=ite;t++){
				temp.push_back(TrainingTok[i][t]);

			}
			FeaTok.push_back(temp);
			temp.push_back(TrainingTok[i][TrainingTok[i].size()-1]);
			MappedTok.push_back(temp);
		}
	}
}
void GenerateUniq(vector<vector<string> >&twoDArr,vector<vector<string> >&arr_Req){
	vector<string>first;
	for(int i=0;i<twoDArr[0].size();i++){
		first.push_back(twoDArr[0][i]);
	}
	arr_Req.push_back(first);


	for(int i=1;i<twoDArr.size();i++){
		vector<string>next_vec;
		bool issame=true;
		for(int j=0;j<twoDArr[i].size();j++){
			if((twoDArr[i][j])!=first[j]){
				issame = false;
				break;
			}
		}
		if(!issame){
			arr_Req.push_back(twoDArr[i]);
			for(int k=0;k<twoDArr[i].size();k++){
				first[k]=twoDArr[i][k];
			}
		}
	}
}
void UpdateTrainedTokens(vector<vector<string> >&TrainingTok,vector<vector<string> >&MappedUniqTok,vector<vector<string> >&FeaUniqTok
		,vector<vector<string> >&Model_Params){
	for(int f=0;f<FeaUniqTok.size();f++){
		vector<string>fea;
		int rep = 0;
		for(int ff=0;ff<FeaUniqTok[f].size();ff++){
			fea.push_back(FeaUniqTok[f][ff]);
		}
		string labelname;
		for(int m=0;m<MappedUniqTok.size();m++){
			bool issame=true;
			for(int mm=0;mm<(MappedUniqTok[m].size()-1);mm++){
				if(MappedUniqTok[m][mm]!=fea[mm]){
					issame = false;
					break;
				}
			}
			if(issame==true){
				labelname = MappedUniqTok[m][MappedUniqTok[m].size()-1];
				rep++;
			}
		}
		if(rep==1){

			vector<string>count_loop;
			for(int ff1=0;ff1<fea.size();ff1++){
				count_loop.push_back(fea[ff1]);
			}

			count_loop.push_back(labelname);
			Model_Params.push_back(count_loop);
			/*			for(int ff1=0;ff1<count_loop.size();ff1++){
				cout<<count_loop[ff1]<<" ";
			}
			cout<<endl;
			 */
			for(int t=0;t<TrainingTok.size();t++){
				if(TrainingTok[t][0]=="-1"){
					bool is_same = true;
					for(int tt=0;tt<FeaUniqTok[f].size();tt++){
						if(FeaUniqTok[f][tt]!=TrainingTok[t][tt+2]){
							is_same = false;
							break;
						}
					}
					if(is_same){

						TrainingTok[t][0]="1";
					}
				}
			}
		}

	}
}
void ReadMFile(char *filename,vector<vector<string > >&Model_Params){
	std::ifstream fileptr(filename);
	string line;
	if (fileptr.is_open()){
		while (getline (fileptr, line))
		{
			istringstream ss(line);
			string item;
			vector<string> tokens;
			while (std::getline(ss, item, ' ')) {
				tokens.push_back(item);
			}
			Model_Params.push_back(tokens);
		}
	}
}
int main(int argc,char **argv){
	vector<vector<string> >Model_Params;

#if TRAINING
	vector<vector<string> >TrainingTok;
	int max_num_tokens=0;
	char trainfilepath[200]="/home/praveen/Documents/MyPersonal/Hackathon/train/train.csv";
	ReadFile(trainfilepath,TrainingTok,max_num_tokens);
	//cout<<max_num_tokens<<endl;

	for(int t=2;t<max_num_tokens;t++){

		vector<vector<string> >MappedTok;
		vector<vector<string> >MappedUniqTok;
		vector<vector<string> >FeaTok;
		vector<vector<string> >FeaUniqTok;
		bool is_hit = false;
		MappedTokens(TrainingTok,MappedTok,FeaTok,t,is_hit);
		if(!is_hit){
			break;
		}
		sort(MappedTok.begin(), MappedTok.end());
		GenerateUniq(MappedTok,MappedUniqTok);

		sort(FeaTok.begin(), FeaTok.end());
		GenerateUniq(FeaTok,FeaUniqTok);

		MappedTok.resize(0);
		FeaTok.resize(0);

		UpdateTrainedTokens(TrainingTok,MappedUniqTok,FeaUniqTok,Model_Params);

	}

	FILE *optfile = fopen("optfile.txt","w");

	for(int i=0;i<Model_Params.size();i++){
		for(int j=0;j<Model_Params[i].size();j++){
			cout<<Model_Params[i][j]<<" ";
			fprintf(optfile,"%s ",Model_Params[i][j].c_str());
		}
		cout<<endl;
		fprintf(optfile,"\n");
	}
	 fclose(optfile);
#endif

	//Update the not updated samples
		for(int t=0;t<TrainingTok.size();t++){
		if(TrainingTok[t][0]=="-1"){
			Model_Params.push_back(TrainingTok[t]);
			TrainingTok[t][0]="1";
		}
	}

#if TESTING
	ReadMFile("optfile.txt",Model_Params);
#endif
	char testfilepath[200]="/home/praveen/Documents/MyPersonal/Hackathon/test_nvPHrOx.csv";
	vector<vector<string> >TestingTok;
	int test_num_token=-1;
	ReadFile(testfilepath,TestingTok,test_num_token);

	for(int i=0;i<TestingTok.size();i++){
		TestingTok[i].push_back("-1");
	}

	for(int t=0;t<TestingTok.size();t++){

		if(TestingTok[t][0]=="-1"){
			bool ismatch = false;
			int mat_id = -1;
			for(int m=0;m<Model_Params.size();m++){
				if((Model_Params[m].size())<(TestingTok[t].size()-1)){
					bool tmp_mat = true;
					for(int mm=0;mm<Model_Params[m].size()-1;mm++){
						if(Model_Params[m][mm]!=TestingTok[t][mm+2]){
							tmp_mat = false;
							break;
						}
					}
					if(tmp_mat==true){
						mat_id = m;
						ismatch = true;
						break;
					}
				}
			}
			if(ismatch){
				TestingTok[t][TestingTok[t].size()-1] = Model_Params[mat_id][Model_Params[mat_id].size()-1];
				TestingTok[t][0]="1";
			}
		}
	}
	//First stage of verification

	//Second stage of verification

	for(int m=0;m<TestingTok.size();m++){
		if(TestingTok[m][0]=="-1"){
			if((TestingTok[m].size()>=3)&&(TestingTok[m][2]=="eurekaselect")&&(TestingTok[m][3]!="node"))	{
				if(atoi(TestingTok[m][3].c_str())<14000){
					TestingTok[m][0]="1";
					TestingTok[m][TestingTok[m].size()-1] ="publication";
				}else{
					TestingTok[m][TestingTok[m].size()-1] ="guidelines";
					TestingTok[m][0]="1";
				}
			}
			if((TestingTok[m].size()>3)&&(TestingTok[m][2]=="aafp")&&(TestingTok[m][3]=="afp")){
				if(atoi(TestingTok[m][4].c_str())<2016){
					TestingTok[m][0]="1";
					TestingTok[m][TestingTok[m].size()-1] ="publication";
				}else{
					TestingTok[m][0]="1";
					TestingTok[m][TestingTok[m].size()-1] ="others";
				}

			}
			if((TestingTok[m].size()>3)&&(TestingTok[m][2]=="ajhp")&&(TestingTok[m][3]=="content")){
				bool isdigit1 = true;
				for(int d=0;d<strlen(TestingTok[m][4].c_str());d++){
					if(!isdigit(TestingTok[m][4][d])){
						isdigit1=false;
						break;
					}
				}
				if(isdigit1){
					TestingTok[m][0]="1";
					TestingTok[m][TestingTok[m].size()-1] ="publication";
				}
			}
			if((TestingTok[m].size()>3)&&(TestingTok[m][2]=="antibiotics")&&(TestingTok[m][3]=="pharmaceuticalconferences")){
				bool isdigit1 = true;
				for(int d=0;d<strlen(TestingTok[m][4].c_str());d++){
					if(!isdigit(TestingTok[m][4][d])){
						isdigit1=false;
						break;
					}
				}
				if(isdigit1){
					TestingTok[m][0]="1";
					TestingTok[m][TestingTok[m].size()-1] ="others";
				}
			}


			if((TestingTok[m].size()>3)&&(TestingTok[m][2]=="bloodjournal")&&(TestingTok[m][3]=="content")){
				bool isdigit1 = true;
				for(int d=0;d<strlen(TestingTok[m][4].c_str());d++){
					if(!isdigit(TestingTok[m][4][d])){
						isdigit1=false;
						break;
					}
				}
				if(isdigit1){
					if(atoi(TestingTok[m][4].c_str())<130){
						TestingTok[m][0]="1";
						TestingTok[m][TestingTok[m].size()-1] ="conferences";
					}else{
						TestingTok[m][0]="1";
						TestingTok[m][TestingTok[m].size()-1] ="news";
						TestingTok[m][TestingTok[m].size()-1] ="news";
					}
				}
			}

			if((TestingTok[m].size()>3)&&(TestingTok[m][2]=="clinsci")&&(TestingTok[m][3]=="content")){
				bool isdigit1 = true;
				for(int d=0;d<strlen(TestingTok[m][4].c_str());d++){
					if(!isdigit(TestingTok[m][4][d])){
						isdigit1=false;
						break;
					}
				}
				if(isdigit1){
					TestingTok[m][0]="1";
					TestingTok[m][TestingTok[m].size()-1] ="publication";
				}
			}
			if((TestingTok[m].size()>3)&&(TestingTok[m][2]=="cvent")&&(TestingTok[m][3]=="events")){
				bool isdigit1 = true;
				for(int d=0;d<strlen(TestingTok[m][4].c_str());d++){
					if(!isdigit(TestingTok[m][4][d])){
						isdigit1=false;
						break;
					}
				}
				if(isdigit1){
					TestingTok[m][0]="1";
					TestingTok[m][TestingTok[m].size()-1] ="news";
				}
			}

			if((TestingTok[m].size()>3)&&(TestingTok[m][2]=="dana")&&(TestingTok[m][3]=="farber")){
				bool isdigit1 = true;
				for(int d=0;d<strlen(TestingTok[m][4].c_str());d++){
					if(!isdigit(TestingTok[m][4][d])){
						isdigit1=false;
						break;
					}
				}
				if(isdigit1){
					TestingTok[m][0]="1";
					TestingTok[m][TestingTok[m].size()-1] ="news";
				}
			}

			if((TestingTok[m].size()>3)&&(TestingTok[m][2]=="europeanreview")&&(TestingTok[m][3]=="article"))	{
				if((atoi(TestingTok[m][4].c_str())>10000)||((atoi(TestingTok[m][4].c_str())>6163)&&(atoi(TestingTok[m][4].c_str())<6900))){
					TestingTok[m][0]="1";
					TestingTok[m][TestingTok[m].size()-1] ="others";
				}else{
					TestingTok[m][0]="1";
					TestingTok[m][TestingTok[m].size()-1] ="publication";
				}
			}

			if((TestingTok[m].size()>3)&&(TestingTok[m][2]=="genetics")&&(TestingTok[m][3]=="content")){
				bool isdigit1 = true;
				for(int d=0;d<strlen(TestingTok[m][4].c_str());d++){
					if(!isdigit(TestingTok[m][4][d])){
						isdigit1=false;
						break;
					}
				}
				if(isdigit1){
					TestingTok[m][0]="1";
					TestingTok[m][TestingTok[m].size()-1] ="publication";
				}
			}

			if((TestingTok[m].size()>3)&&(TestingTok[m][2]=="hormones")&&(TestingTok[m][3]=="gr")){
				if(atoi(TestingTok[m][4].c_str())>100){
					TestingTok[m][0]="1";
					TestingTok[m][TestingTok[m].size()-1] ="publication";
				}
			}

			if((TestingTok[m].size()>4)&&(TestingTok[m][2]=="agelab")&&(TestingTok[m][3]=="mit")&&(TestingTok[m][4]=="edu")){
				bool isdigit1 = true;

				for(int d=0;d<strlen(TestingTok[m][5].c_str());d++){
					if(!isdigit(TestingTok[m][5][d])){
						isdigit1=false;
						break;
					}
				}
				if((isdigit1)&&(strlen(TestingTok[m][5].c_str())==4)){
					TestingTok[m][0]="1";
					TestingTok[m][TestingTok[m].size()-1] ="publication";
				}
			}

			if((TestingTok[m].size()>4)&&(TestingTok[m][2]=="aim")&&(TestingTok[m][3]=="bmj")&&(TestingTok[m][4]=="content")){
				bool isdigit1 = true;

				for(int d=0;d<strlen(TestingTok[m][5].c_str());d++){
					if(!isdigit(TestingTok[m][5][d])){
						isdigit1=false;
						break;
					}
				}
				if((isdigit1)){
					TestingTok[m][0]="1";
					TestingTok[m][TestingTok[m].size()-1] ="publication";
				}
			}

			if((TestingTok[m].size()>4)&&(TestingTok[m][2]=="asheducationbook")&&(TestingTok[m][3]=="hematologylibrary")&&(TestingTok[m][4]=="content")){
				bool isdigit1 = true;

				for(int d=0;d<strlen(TestingTok[m][5].c_str());d++){
					if(!isdigit(TestingTok[m][5][d])){
						isdigit1=false;
						break;
					}
				}
				if((isdigit1)){
					TestingTok[m][0]="1";
					TestingTok[m][TestingTok[m].size()-1] ="publication";
				}
			}
			if((TestingTok[m].size()>4)&&(TestingTok[m][2]=="bio")&&(TestingTok[m][3]=="biologists")&&(TestingTok[m][4]=="content")){
				bool isdigit1 = true;

				for(int d=0;d<strlen(TestingTok[m][5].c_str());d++){
					if(!isdigit(TestingTok[m][5][d])){
						isdigit1=false;
						break;
					}
				}
				if((isdigit1)){
					TestingTok[m][0]="1";
					TestingTok[m][TestingTok[m].size()-1] ="publication";
				}
			}
			if((TestingTok[m].size()>4)&&(TestingTok[m][2]=="blogs")&&(TestingTok[m][3]=="novonordisk")&&(TestingTok[m][4]=="graduates")){
				bool isdigit1 = true;

				for(int d=0;d<strlen(TestingTok[m][5].c_str());d++){
					if(!isdigit(TestingTok[m][5][d])){
						isdigit1=false;
						break;
					}
				}
				if((isdigit1)){
					TestingTok[m][0]="1";
					TestingTok[m][TestingTok[m].size()-1] ="others";
				}
			}
			if((TestingTok[m].size()>4)&&(TestingTok[m][2]=="cancerdiscovery")&&(TestingTok[m][3]=="aacrjournals")&&(TestingTok[m][4]=="content")){
				bool isdigit1 = true;

				for(int d=0;d<strlen(TestingTok[m][5].c_str());d++){
					if(!isdigit(TestingTok[m][5][d])){
						isdigit1=false;
						break;
					}
				}
				if((isdigit1)){
					if((atoi(TestingTok[m][5].c_str())<6)){
						TestingTok[m][0]="1";
						TestingTok[m][TestingTok[m].size()-1] ="publication";
					}else{
						TestingTok[m][0]="1";
						TestingTok[m][TestingTok[m].size()-1] ="news";
					}
				}
			}
			if((TestingTok[m].size()>4)&&(TestingTok[m][2]=="cancerimmunolres")&&(TestingTok[m][3]=="aacrjournals")&&(TestingTok[m][4]=="content")){
				bool isdigit1 = true;

				for(int d=0;d<strlen(TestingTok[m][5].c_str());d++){
					if(!isdigit(TestingTok[m][5][d])){
						isdigit1=false;
						break;
					}
				}
				if((isdigit1)){
					TestingTok[m][0]="1";
					TestingTok[m][TestingTok[m].size()-1] ="publication";
				}
			}

			if((TestingTok[m].size()>4)&&(TestingTok[m][2]=="cancerpreventionresearch")&&(TestingTok[m][3]=="aacrjournals")&&(TestingTok[m][4]=="content")){
				bool isdigit1 = true;

				for(int d=0;d<strlen(TestingTok[m][5].c_str());d++){
					if(!isdigit(TestingTok[m][5][d])){
						isdigit1=false;
						break;
					}
				}
				if((isdigit1)){
					TestingTok[m][0]="1";
					TestingTok[m][TestingTok[m].size()-1] ="publication";
				}
			}

			if((TestingTok[m].size()>4)&&(TestingTok[m][2]=="cancerres")&&(TestingTok[m][3]=="aacrjournals")&&(TestingTok[m][4]=="content")){
				bool isdigit1 = true;

				for(int d=0;d<strlen(TestingTok[m][5].c_str());d++){
					if(!isdigit(TestingTok[m][5][d])){
						isdigit1=false;
						break;
					}
				}
				if((isdigit1)){
					TestingTok[m][0]="1";
					TestingTok[m][TestingTok[m].size()-1] ="publication";
				}
			}


			if((TestingTok[m].size()>4)&&(TestingTok[m][2]=="care")&&(TestingTok[m][3]=="diabetesjournals")&&(TestingTok[m][4]=="content")){
				bool isdigit1 = true;

				for(int d=0;d<strlen(TestingTok[m][5].c_str());d++){
					if(!isdigit(TestingTok[m][5][d])){
						isdigit1=false;
						break;
					}
				}
				if((isdigit1)){
					if((atoi(TestingTok[m][5].c_str())<40)){
						TestingTok[m][0]="1";
						TestingTok[m][TestingTok[m].size()-1] ="publication";
					}else{
						TestingTok[m][0]="1";
						TestingTok[m][TestingTok[m].size()-1] ="news";
					}
				}
			}
			if((TestingTok[m].size()>4)&&(TestingTok[m][2]=="careers")&&(TestingTok[m][3]=="amgen")&&(TestingTok[m][4]=="job")){
				bool isdigit1 = true;

				for(int d=0;d<strlen(TestingTok[m][5].c_str());d++){
					if(!isdigit(TestingTok[m][5][d])){
						isdigit1=false;
						break;
					}
				}
				if((isdigit1)){
					TestingTok[m][0]="1";
					TestingTok[m][TestingTok[m].size()-1] ="others";
				}
			}
			if((TestingTok[m].size()>4)&&(TestingTok[m][2]=="casereports")&&(TestingTok[m][3]=="bmj")&&(TestingTok[m][4]=="content")){
				bool isdigit1 = true;

				for(int d=0;d<strlen(TestingTok[m][5].c_str());d++){
					if(!isdigit(TestingTok[m][5][d])){
						isdigit1=false;
						break;
					}
				}
				if((isdigit1)){
					TestingTok[m][0]="1";
					TestingTok[m][TestingTok[m].size()-1] ="publication";
				}
			}
			if((TestingTok[m].size()>4)&&(TestingTok[m][2]=="cebp")&&(TestingTok[m][3]=="aacrjournals")&&(TestingTok[m][4]=="content")){
				bool isdigit1 = true;

				for(int d=0;d<strlen(TestingTok[m][5].c_str());d++){
					if(!isdigit(TestingTok[m][5][d])){
						isdigit1=false;
						break;
					}
				}
				if((isdigit1)){
					TestingTok[m][0]="1";
					TestingTok[m][TestingTok[m].size()-1] ="publication";
				}
			}

			if((TestingTok[m].size()>4)&&(TestingTok[m][2]=="circ")&&(TestingTok[m][3]=="ahajournals")&&(TestingTok[m][4]=="content")){
				bool isdigit1 = true;

				for(int d=0;d<strlen(TestingTok[m][5].c_str());d++){
					if(!isdigit(TestingTok[m][5][d])){
						isdigit1=false;
						break;
					}
				}
				if((isdigit1)){
					TestingTok[m][0]="1";
					TestingTok[m][TestingTok[m].size()-1] ="publication";
				}
			}
			if((TestingTok[m].size()>4)&&(TestingTok[m].size()>4)&&(TestingTok[m][2]=="clincancerres")&&(TestingTok[m][3]=="aacrjournals")&&(TestingTok[m][4]=="content")){
				bool isdigit1 = true;

				for(int d=0;d<strlen(TestingTok[m][5].c_str());d++){
					if(!isdigit(TestingTok[m][5][d])){
						isdigit1=false;
						break;
					}
				}
				if((isdigit1)){
					if((atoi(TestingTok[m][5].c_str())<23)){
						TestingTok[m][0]="1";
						TestingTok[m][TestingTok[m].size()-1] ="publication";
					}else{
						TestingTok[m][0]="1";
						TestingTok[m][TestingTok[m].size()-1] ="news";
					}
				}
			}
			if((TestingTok[m].size()>4)&&(TestingTok[m][2]=="cmaj")&&(TestingTok[m][3]=="ca")&&(TestingTok[m][4]=="content")){
				bool isdigit1 = true;

				for(int d=0;d<strlen(TestingTok[m][5].c_str());d++){
					if(!isdigit(TestingTok[m][5][d])){
						isdigit1=false;
						break;
					}
				}
				if((isdigit1)){
					if((atoi(TestingTok[m][5].c_str())<188)){
						TestingTok[m][0]="1";
						TestingTok[m][TestingTok[m].size()-1] ="publication";
					}else{
						TestingTok[m][0]="1";
						TestingTok[m][TestingTok[m].size()-1] ="profile";
					}
				}
			}
			if((TestingTok[m].size()>4)&&(TestingTok[m][2]=="community")&&(TestingTok[m][3]=="beatingbowelcancer")&&(TestingTok[m][4]=="forum")){
				bool isdigit1 = true;

				for(int d=0;d<strlen(TestingTok[m][5].c_str());d++){
					if(!isdigit(TestingTok[m][5][d])){
						isdigit1=false;
						break;
					}
				}
				if((isdigit1)){
					TestingTok[m][0]="1";
					TestingTok[m][TestingTok[m].size()-1] ="forum";
				}
			}

			if((TestingTok[m].size()>4)&&(TestingTok[m][2]=="erj")&&(TestingTok[m][3]=="ersjournals")&&(TestingTok[m][4]=="content")){
				bool isdigit1 = true;

				for(int d=0;d<strlen(TestingTok[m][5].c_str());d++){
					if(!isdigit(TestingTok[m][5][d])){
						isdigit1=false;
						break;
					}
				}
				if((isdigit1)){
					if((atoi(TestingTok[m][5].c_str())<46)){
						TestingTok[m][0]="1";
						TestingTok[m][TestingTok[m].size()-1] ="publication";
					}else{
						TestingTok[m][0]="1";
						TestingTok[m][TestingTok[m].size()-1] ="guidelines";
					}
				}
			}
			if((TestingTok[m].size()>4)&&(TestingTok[m][2]=="err")&&(TestingTok[m][3]=="ersjournals")&&(TestingTok[m][4]=="content")){
				bool isdigit1 = true;

				for(int d=0;d<strlen(TestingTok[m][5].c_str());d++){
					if(!isdigit(TestingTok[m][5][d])){
						isdigit1=false;
						break;
					}
				}
				if((isdigit1)){
					TestingTok[m][0]="1";
					TestingTok[m][TestingTok[m].size()-1] ="publication";
				}
			}
			if((TestingTok[m].size()>4)&&(TestingTok[m][2]=="esmoopen")&&(TestingTok[m][3]=="bmj")&&(TestingTok[m][4]=="content")){
				bool isdigit1 = true;

				for(int d=0;d<strlen(TestingTok[m][5].c_str());d++){
					if(!isdigit(TestingTok[m][5][d])){
						isdigit1=false;
						break;
					}
				}
				if((isdigit1)){
					TestingTok[m][0]="1";
					TestingTok[m][TestingTok[m].size()-1] ="publication";
				}
			}
			if((TestingTok[m].size()>4)&&(TestingTok[m][2]=="gut")&&(TestingTok[m][3]=="bmj")&&(TestingTok[m][4]=="content")){
				bool isdigit1 = true;

				for(int d=0;d<strlen(TestingTok[m][5].c_str());d++){
					if(!isdigit(TestingTok[m][5][d])){
						isdigit1=false;
						break;
					}
				}
				if((isdigit1)){
					TestingTok[m][0]="1";
					TestingTok[m][TestingTok[m].size()-1] ="publication";
				}else{
					TestingTok[m][0]="1";
					TestingTok[m][TestingTok[m].size()-1] ="guidelines";
				}
			}
			if((TestingTok[m].size()>4)&&(TestingTok[m][2]=="jcs")&&(TestingTok[m][3]=="biologists")&&(TestingTok[m][4]=="content")){
				bool isdigit1 = true;

				for(int d=0;d<strlen(TestingTok[m][5].c_str());d++){
					if(!isdigit(TestingTok[m][5][d])){
						isdigit1=false;
						break;
					}
				}
				if((isdigit1)){
					if((atoi(TestingTok[m][5].c_str())<129)){
						TestingTok[m][0]="1";
						TestingTok[m][TestingTok[m].size()-1] ="publication";
					}else{
						TestingTok[m][0]="1";
						TestingTok[m][TestingTok[m].size()-1] ="profile";
					}
				}else{
					TestingTok[m][0]="1";
					TestingTok[m][TestingTok[m].size()-1] ="profile";
				}
			}
			if((TestingTok[m].size()>4)&&(TestingTok[m][2]=="jgp")&&(TestingTok[m][3]=="rupress")&&(TestingTok[m][4]=="content")){
				bool isdigit1 = true;

				for(int d=0;d<strlen(TestingTok[m][5].c_str());d++){
					if(!isdigit(TestingTok[m][5][d])){
						isdigit1=false;
						break;
					}
				}
				if((isdigit1)){
					TestingTok[m][0]="1";
					TestingTok[m][TestingTok[m].size()-1] ="publication";
				}else{
					TestingTok[m][0]="1";
					TestingTok[m][TestingTok[m].size()-1] ="others";
				}
			}

			if((TestingTok[m].size()>4)&&(TestingTok[m][2]=="msb")&&(TestingTok[m][3]=="embopress")&&(TestingTok[m][4]=="content")){
				bool isdigit1 = true;

				for(int d=0;d<strlen(TestingTok[m][5].c_str());d++){
					if(!isdigit(TestingTok[m][5][d])){
						isdigit1=false;
						break;
					}
				}
				if((isdigit1)){
					TestingTok[m][0]="1";
					TestingTok[m][TestingTok[m].size()-1] ="publication";
				}
			}
			if((TestingTok[m].size()>4)&&(TestingTok[m][2]=="news")&&(TestingTok[m][3]=="mit")&&(TestingTok[m][4]=="edu")){
				bool isdigit1 = true;

				for(int d=0;d<strlen(TestingTok[m][5].c_str());d++){
					if(!isdigit(TestingTok[m][5][d])){
						isdigit1=false;
						break;
					}
				}
				if((isdigit1)){
					TestingTok[m][0]="1";
					TestingTok[m][TestingTok[m].size()-1] ="news";
				}
			}


			if((TestingTok[m].size()>4)&&(TestingTok[m][2]=="nice")&&(TestingTok[m][3]=="uk")&&(TestingTok[m][4]=="guidance")){
				if(isupper(TestingTok[m][5][0])){
					TestingTok[m][0]="1";
					TestingTok[m][TestingTok[m].size()-1] ="news";
				}else{
					TestingTok[m][0]="1";
					TestingTok[m][TestingTok[m].size()-1] ="guidelines";
				}
			}

			if((TestingTok[m].size()>4)&&(TestingTok[m][2]=="rsbl")&&(TestingTok[m][3]=="royalsocietypublishing")&&(TestingTok[m][4]=="content")){
				bool isdigit1 = true;

				for(int d=0;d<strlen(TestingTok[m][5].c_str());d++){
					if(!isdigit(TestingTok[m][5][d])){
						isdigit1=false;
						break;
					}
				}
				if((isdigit1)){
					TestingTok[m][0]="1";
					TestingTok[m][TestingTok[m].size()-1] ="publication";
				}else{
					TestingTok[m][0]="1";
					TestingTok[m][TestingTok[m].size()-1] ="others";
				}
			}


			if((TestingTok[m].size()>4)&&(TestingTok[m][2]=="rspa")&&(TestingTok[m][3]=="royalsocietypublishing")&&(TestingTok[m][4]=="content")){
				bool isdigit1 = true;

				for(int d=0;d<strlen(TestingTok[m][5].c_str());d++){
					if(!isdigit(TestingTok[m][5][d])){
						isdigit1=false;
						break;
					}
				}
				if((isdigit1)){
					TestingTok[m][0]="1";
					TestingTok[m][TestingTok[m].size()-1] ="publication";
				}else{
					TestingTok[m][0]="1";
					TestingTok[m][TestingTok[m].size()-1] ="others";
				}
			}

			if((TestingTok[m].size()>4)&&(TestingTok[m][2]=="slctr")&&(TestingTok[m][3]=="lk")&&(TestingTok[m][4]=="trials")){
				bool isdigit1 = true;

				for(int d=0;d<strlen(TestingTok[m][5].c_str());d++){
					if(!isdigit(TestingTok[m][5][d])){
						isdigit1=false;
						break;
					}
				}
				if((isdigit1)){
					TestingTok[m][0]="1";
					TestingTok[m][TestingTok[m].size()-1] ="clinicalTrials";
				}else{
					TestingTok[m][0]="1";
					TestingTok[m][TestingTok[m].size()-1] ="others";
				}
			}

			if((TestingTok[m].size()>4)&&(TestingTok[m][2]=="thorax")&&(TestingTok[m][3]=="bmj")&&(TestingTok[m][4]=="content")){
				bool isdigit1 = true;

				for(int d=0;d<strlen(TestingTok[m][5].c_str());d++){
					if(!isdigit(TestingTok[m][5][d])){
						isdigit1=false;
						break;
					}
				}
				if((isdigit1)){
					TestingTok[m][0]="1";
					TestingTok[m][TestingTok[m].size()-1] ="conferences";
				}else{
					TestingTok[m][0]="1";
					TestingTok[m][TestingTok[m].size()-1] ="others";
				}
			}
			if((TestingTok[m].size()>3)&&(TestingTok[m][2]=="blogs")&&(TestingTok[m][3]=="biomedcentral")){
				TestingTok[m][0]="1";
				TestingTok[m][TestingTok[m].size()-1] ="news";
			}
			if((TestingTok[m].size()>5)&&(TestingTok[m][2]=="canjurol")&&(TestingTok[m][3]=="abstract")&&(TestingTok[m][4]=="php")&&(TestingTok[m][5]=="ArticleID")){
				bool isdigit1 = true;

				for(int d=0;d<strlen(TestingTok[m][6].c_str());d++){
					if(!isdigit(TestingTok[m][6][d])){
						isdigit1=false;
						break;
					}
				}
				if((isdigit1)){
					if(atoi(TestingTok[m][6].c_str())<3140){
						TestingTok[m][0]="1";
						TestingTok[m][TestingTok[m].size()-1] ="others";
					}else{
						TestingTok[m][0]="1";
						TestingTok[m][TestingTok[m].size()-1] ="publication";
					}

				}else{
					TestingTok[m][0]="1";
					TestingTok[m][TestingTok[m].size()-1] ="publication";
				}
			}

			if(((TestingTok[m].size()>5)&&TestingTok[m][2]=="childrenshospital")&&(TestingTok[m][3]=="news")&&(TestingTok[m][4]=="and")&&(TestingTok[m][5]=="events")){
				bool isdigit1 = true;

				for(int d=0;d<strlen(TestingTok[m][6].c_str());d++){
					if(!isdigit(TestingTok[m][6][d])){
						isdigit1=false;
						break;
					}
				}
				if((isdigit1)){
					TestingTok[m][0]="1";
					TestingTok[m][TestingTok[m].size()-1] ="news";
				}else{
					TestingTok[m][0]="1";
					TestingTok[m][TestingTok[m].size()-1] ="others";
				}
			}

			if((TestingTok[m].size()>5)&&(TestingTok[m].size()>5)&&(TestingTok[m][2]=="clinicaltrials")&&(TestingTok[m][3]=="gov")&&(TestingTok[m][4]=="ct2")&&(TestingTok[m][5]=="show")){
				TestingTok[m][0]="1";
				TestingTok[m][TestingTok[m].size()-1] ="clinicalTrials";
			}
			if((TestingTok[m].size()>5)&&(TestingTok[m][2]=="linkinghub")&&(TestingTok[m][3]=="elsevier")&&(TestingTok[m][4]=="retrieve")&&(TestingTok[m][5]=="pii")){
				if(strlen(TestingTok[m][6].c_str())>5){
					TestingTok[m][0]="1";
					TestingTok[m][TestingTok[m].size()-1] ="guidelines";
				}else{
					TestingTok[m][0]="1";
					TestingTok[m][TestingTok[m].size()-1] ="publication";
				}
				TestingTok[m][0]="1";
				TestingTok[m][TestingTok[m].size()-1] ="clinicalTrials";
			}
			if((TestingTok[m].size()>5)&&(TestingTok[m][2]=="thesis")&&(TestingTok[m][3]=="library")&&(TestingTok[m][4]=="caltech")&&(TestingTok[m][5]=="edu")){
				bool isdigit1 = true;

				for(int d=0;d<strlen(TestingTok[m][6].c_str());d++){
					if(!isdigit(TestingTok[m][6][d])){
						isdigit1=false;
						break;
					}
				}
				if((isdigit1)){
					TestingTok[m][0]="1";
					TestingTok[m][TestingTok[m].size()-1] ="thesis";
				}else{
					TestingTok[m][0]="1";
					TestingTok[m][TestingTok[m].size()-1] ="others";
				}
			}
			if((TestingTok[m].size()>5)&&(TestingTok[m][2]=="theunion")&&(TestingTok[m][3]=="what")&&(TestingTok[m][4]=="we")&&(TestingTok[m][5]=="do")){

				if((TestingTok[m][6]=="conferences")){
					TestingTok[m][0]="1";
					TestingTok[m][TestingTok[m].size()-1] ="conferences";
				}else{
					TestingTok[m][0]="1";
					TestingTok[m][TestingTok[m].size()-1] ="others";
				}

			}

			if((TestingTok[m].size()>5)&&(TestingTok[m][2]=="tkd")&&(TestingTok[m][3]=="tr")&&(TestingTok[m][4]=="en")&&(TestingTok[m][5]=="menu")){

				if((atoi(TestingTok[m][6].c_str())<28)&&(atoi(TestingTok[m][6].c_str())>4)){
					TestingTok[m][0]="1";
					TestingTok[m][TestingTok[m].size()-1] ="profile";
				}else{
					TestingTok[m][0]="1";
					TestingTok[m][TestingTok[m].size()-1] ="others";
				}

			}


			if((TestingTok[m].size()>5)&&(TestingTok[m][2]=="wiley")&&(TestingTok[m][3]=="WileyCDA")&&(TestingTok[m][4]=="Section")&&(TestingTok[m][5]=="id")){

				if((atoi(TestingTok[m][6].c_str())>=822613)&&(atoi(TestingTok[m][6].c_str())<829000)){
					TestingTok[m][0]="1";
					TestingTok[m][TestingTok[m].size()-1] ="profile";
				}else{
					TestingTok[m][0]="1";
					TestingTok[m][TestingTok[m].size()-1] ="others";
				}

			}


			if((TestingTok[m].size()>5)&&(TestingTok[m][2]=="blog")&&(TestingTok[m][3]=="dana")&&(TestingTok[m][4]=="farber")&&(TestingTok[m][5]=="insight")){
				bool isdigit1 = true;

				for(int d=0;d<strlen(TestingTok[m][6].c_str());d++){
					if(!isdigit(TestingTok[m][6][d])){
						isdigit1=false;
						break;
					}
				}
				if((isdigit1)){
					TestingTok[m][0]="1";
					TestingTok[m][TestingTok[m].size()-1] ="others";
				}else{
					TestingTok[m][0]="1";
					TestingTok[m][TestingTok[m].size()-1] ="publication";
				}
			}

			if((TestingTok[m].size()>5)&&(TestingTok[m][2]=="blogs")&&(TestingTok[m][3]=="biomedcentral")&&(TestingTok[m][4]=="on")&&(TestingTok[m][5]=="medicine")){
				bool isdigit1 = true;
				bool isdigit2 = true;
				for(int d=0;d<strlen(TestingTok[m][6].c_str());d++){
					if(!isdigit(TestingTok[m][6][d])){
						isdigit1=false;
						break;
					}
				}
				for(int d=0;d<strlen(TestingTok[m][7].c_str());d++){
					if(!isdigit(TestingTok[m][7][d])){
						isdigit2=false;
						break;
					}
				}
				if((atoi(TestingTok[m][6].c_str())<=2017)&&(atoi(TestingTok[m][7].c_str())<10)){
					TestingTok[m][0]="1";
					TestingTok[m][TestingTok[m].size()-1] ="news";
				}else{
					TestingTok[m][0]="1";
					TestingTok[m][TestingTok[m].size()-1] ="others";
				}

			}

			if((TestingTok[m].size()>5)&&(TestingTok[m].size()>4)&&(TestingTok[m][2]=="link")&&(TestingTok[m][3]=="springer")&&(TestingTok[m][4]=="article")){
				TestingTok[m][0]="1";
				TestingTok[m][TestingTok[m].size()-1] ="guidelines";
			}



			if((TestingTok[m].size()>5)&&(TestingTok[m][2]=="mallinckrodt")&&(TestingTok[m][3]=="about")&&(TestingTok[m][4]=="news")&&(TestingTok[m][5]=="and")){
				bool isdigit1 = true;

				for(int d=0;d<strlen(TestingTok[m][6].c_str());d++){
					if(!isdigit(TestingTok[m][6][d])){
						isdigit1=false;
						break;
					}
				}
				if((isdigit1)){
					TestingTok[m][0]="1";
					TestingTok[m][TestingTok[m].size()-1] ="news";
				}else{
					TestingTok[m][0]="1";
					TestingTok[m][TestingTok[m].size()-1] ="profile";
				}
			}
			if((TestingTok[m].size()>5)&&(TestingTok[m][2]=="ncbi")&&(TestingTok[m][3]=="nlm")&&(TestingTok[m][4]=="nih")&&(TestingTok[m][5]=="gov")){
				TestingTok[m][0]="1";
				TestingTok[m][TestingTok[m].size()-1] ="guidelines";
			}
			if((TestingTok[m].size()>=2)&&(TestingTok[m][2]=="nejm")){
				if(strlen(TestingTok[m][7].c_str())>15){
					TestingTok[m][0]="1";
					TestingTok[m][TestingTok[m].size()-1] ="publication";
				}else{
					TestingTok[m][0]="1";
					TestingTok[m][TestingTok[m].size()-1] ="news";
				}
			}
			if((TestingTok[m].size()>=2)&&(TestingTok[m][2]=="tandfonline")){
				TestingTok[m][0]="1";
				TestingTok[m][TestingTok[m].size()-1] ="publication";
			}

			if((TestingTok[m].size()>=3)&&(TestingTok[m][2]=="blogs")&&(TestingTok[m][3]=="fda")){
				bool isdigit1 = true;

				for(int d=0;d<strlen(TestingTok[m][8].c_str());d++){
					if(!isdigit(TestingTok[m][8][d])){
						isdigit1=false;
						break;
					}
				}
				if((isdigit1)){
					TestingTok[m][0]="1";
					TestingTok[m][TestingTok[m].size()-1] ="news";
				}else{
					TestingTok[m][0]="1";
					TestingTok[m][TestingTok[m].size()-1] ="others";
				}
			}


			if((TestingTok[m].size()>=3)&&(TestingTok[m][2]=="journals")&&(TestingTok[m][3]=="sagepub")){
				TestingTok[m][0]="1";
				TestingTok[m][TestingTok[m].size()-1] ="publication";
			}
			if((TestingTok[m].size()>=3)&&(TestingTok[m][2]=="jfootankleres")&&(TestingTok[m][3]=="biomedcentral")){
				TestingTok[m][0]="1";
				TestingTok[m][TestingTok[m].size()-1] ="conferences";
			}
			if((TestingTok[m].size()>=3)&&(TestingTok[m][2]=="doctors")&&(TestingTok[m][3]=="dana")){
				TestingTok[m][0]="1";
				TestingTok[m][TestingTok[m].size()-1] ="others";
			}

			if((TestingTok[m].size()>5)&&(TestingTok[m][2]=="ped")&&(TestingTok[m][3]=="rheum")&&(TestingTok[m][4]=="biomedcentral")&&(TestingTok[m][5]=="articles")){
				TestingTok[m][0]="1";
				TestingTok[m][TestingTok[m].size()-1] ="conferences";
			}

		}
	}
	//Now we have to apply the voting
	for(int i=0;i<TestingTok.size();i++){
		if(TestingTok[i][0]=="-1"){
			int max_val = -20;
			int model_index = -20;
			int class_max_cnt_index=-10;
			int class_max_cnt_val=-10;
		/*	clinicalTrials
			conferences
			forum
			guidelines
			news
			others
			profile
			publication
			thesis*/
			string class_names[10];
			class_names[0]="clinicalTrials";
			class_names[1]="conferences";
			class_names[2]="forum";
			class_names[3]="guidelines";
			class_names[4]="news";
			class_names[5]="others";
			class_names[6]="profile";
			class_names[7]="publication";
			class_names[8]="thesis";
			int class_cnts[10];
			for(int c=0;c<9;c++){
				class_cnts[c] =0;
			}

			for(int m=0;m<Model_Params.size();m++){
				int counter=0;
				for(int j=2;j<TestingTok[i].size();j++){
					for(int mm=0;mm<(Model_Params[m].size()-1);mm++){
						if(TestingTok[i][j]==Model_Params[m][mm]){
							counter++;
							if(Model_Params[m][(Model_Params[m].size()-1)]=="clinicalTrials"){
								class_cnts[0]=class_cnts[0]+1;
								if(class_cnts[0]>class_max_cnt_val){
									class_max_cnt_val+=1;
									class_max_cnt_index =0;
								}
							}
							if(Model_Params[m][(Model_Params[m].size()-1)]=="conferences"){
								class_cnts[1]=class_cnts[1]+1;
								if(class_cnts[1]>class_max_cnt_val){
									class_max_cnt_val+=1;
									class_max_cnt_index =1;
								}
							}
							if(Model_Params[m][(Model_Params[m].size()-1)]=="forum"){
								class_cnts[2]=class_cnts[2]+1;
								if(class_cnts[2]>class_max_cnt_val){
									class_max_cnt_val+=1;
									class_max_cnt_index =2;
								}
							}
							if(Model_Params[m][(Model_Params[m].size()-1)]=="guidelines"){
								class_cnts[3]=class_cnts[3]+1;
								if(class_cnts[3]>class_max_cnt_val){
									class_max_cnt_val+=1;
									class_max_cnt_index =3;
								}
							}
							if(Model_Params[m][(Model_Params[m].size()-1)]=="news"){
								class_cnts[4]=class_cnts[4]+1;
								if(class_cnts[4]>class_max_cnt_val){
									class_max_cnt_val+=1;
									class_max_cnt_index =4;
								}
							}
							if(Model_Params[m][(Model_Params[m].size()-1)]=="others"){
								class_cnts[5]=class_cnts[5]+1;
								if(class_cnts[5]>class_max_cnt_val){
									class_max_cnt_val+=1;
									class_max_cnt_index =5;
								}
							}
							if(Model_Params[m][(Model_Params[m].size()-1)]=="profile"){
								class_cnts[6]=class_cnts[6]+1;
								if(class_cnts[6]>class_max_cnt_val){
									class_max_cnt_val+=1;
									class_max_cnt_index =6;
								}
							}
							if(Model_Params[m][(Model_Params[m].size()-1)]=="publication"){
								class_cnts[7]=class_cnts[7]+1;
								if(class_cnts[7]>class_max_cnt_val){
									class_max_cnt_val+=1;
									class_max_cnt_index =7;
								}
							}
							if(Model_Params[m][(Model_Params[m].size()-1)]=="thesis"){
								class_cnts[8]=class_cnts[8]+1;
								if(class_cnts[8]>class_max_cnt_val){
									class_max_cnt_val+=1;
									class_max_cnt_index =8;
								}
							}
						}
					}
				}
				if(counter>max_val){
					max_val = counter;
					model_index = m;
				}
			}

			if(max_val!=20){
				TestingTok[i][(TestingTok[i].size()-1)] = class_names[class_max_cnt_index];
				TestingTok[i][0]="1";
			}
		}

	}
for(int i=0;i<TestingTok.size();i++){
	if(TestingTok[i][TestingTok[i].size()-1]==" "){
		TestingTok[i][TestingTok[i].size()-1] = "others";
	}
	cout<<TestingTok[i][1]<<" "<<TestingTok[i][TestingTok[i].size()-1]<<endl;
}


	return 0;
}
