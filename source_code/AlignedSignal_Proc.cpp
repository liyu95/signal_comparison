#include <string>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <cmath>
#include <map>
using namespace std;

//-------- utility ------//
void getBaseName(string &in,string &out,char slash,char dot)
{
	int i,j;
	int len=(int)in.length();
	for(i=len-1;i>=0;i--)
	{
		if(in[i]==slash)break;
	}
	i++;
	for(j=len-1;j>=0;j--)
	{
		if(in[j]==dot)break;
	}
	if(j==-1)j=len;
	out=in.substr(i,j-i);
}
void getRootName(string &in,string &out,char slash)
{
	int i;
	int len=(int)in.length();
	for(i=len-1;i>=0;i--)
	{
		if(in[i]==slash)break;
	}
	if(i<=0)out=".";
	else out=in.substr(0,i);
}

//--------- parse string --------------//
int Parse_Str(string &in,vector <string> &out)
{
	istringstream www(in);
	out.clear();
	int count=0;
	for(;;)
	{
		string buf;
		if(! (www>>buf) )break;
		out.push_back(buf);
		count++;
	}
	return count;
}

//------------ load signal ------------//
//-> example
/*
478
571
531
542
536
541
534
549
*/
int Load_Signal(string &input, vector <double> &signal)
{
	ifstream fin;
	string buf,temp;
	//read
	fin.open(input.c_str(), ios::in);
	if(fin.fail()!=0)
	{
		fprintf(stderr,"file %s not found!\n",input.c_str());
		exit(-1);
	}
	//load
	signal.clear();
	double raw_signal;
	int count=0;
	for(;;)
	{
		if(! (fin>>raw_signal) )break;
		signal.push_back(raw_signal);
		//count++
		count++;
	}
	//return
	return count;
}


//------------ load aligned signals ----//
//-> example from cwDTW
/*
         1          2                3               4             5               6                            7
------------------------------------------------------------------------------------------------------------------------
         1          1 |             316             550 |        -1.16628         1.04565          diff:        2.21193
         2          2 |             347             433 |       -0.526507       -0.560209          diff:       0.033702
         3          3 |             319             411 |        -1.10436       -0.862165          diff:       0.242199
         3          4 |             319             409 |        -1.10436       -0.889616          diff:       0.214749
         3          5 |             319             408 |        -1.10436       -0.903341          diff:       0.201023
         3          6 |             319             389 |        -1.10436        -1.16412          diff:      0.0597573
         3          7 |             319             389 |        -1.10436        -1.16412          diff:      0.0597573
         3          8 |             319             377 |        -1.10436        -1.32883          diff:       0.224461
         3          9 |             319             391 |        -1.10436        -1.13667          diff:      0.0323067

---------
[Legend]:
---------

the 1st column shows the mapping of the first position (e.g., on expected signal) starting from 1,
the 2nd column shows the mapping of the second position (e.g., on raw signal) starting from 1,

the 3rd column displays the original value of the first input signal (e.g., on expected signal),
the 4th column displays the original value of the second input signal (e.g., on raw signal),

the 5th column indicates the Z-normalized value of the first input signal (e.g., on expected signal),
the 6th column indicates the Z-normalized value of the second input signal (e.g., on raw signal),

the 7th column illustrates the absolute difference between the two Z-normalized values.


*/

//-> [note]: we ALWAYS regard the 2nd signal as 'raw signal', and output it as first
int Load_cwDTW_alignment_nostr(string &infile,
	vector <pair<int,int> > &alignment,
	vector <double> &raw_signal, vector <double> &expected_signal, 
	vector <double> &zsco_1, vector <double> &zsco_2, vector <double> &zsco_diff)
{
	//read
	ifstream fin;
	string buf,temp;
	fin.open(infile.c_str(), ios::in);
	if(fin.fail()!=0)
	{
		fprintf(stderr,"list %s not found!!\n",infile.c_str());
		exit(-1);
	}
	//init
	alignment.clear();
	raw_signal.clear();
	expected_signal.clear();
	zsco_1.clear();
	zsco_2.clear();
	zsco_diff.clear();
	int col_size=10;
	//load
	int count=0;
	int first=1;
	int start1=0;
	int start2=0;
	for(;;)
	{
		if(!getline(fin,buf,'\n'))break;
		vector <string> out;
		int retv=Parse_Str(buf,out);
		if(retv!=col_size)
		{
			fprintf(stderr,"format bad at %d -> retv %d not equal to col_size %d\n",
				count,retv,col_size);
			exit(-1);
		}
		//start
		int pos1=atoi(out[1].c_str())-1;
		int pos2=atoi(out[0].c_str())-1;
		if(first==1)
		{
			first=0;
			start1=pos1;
			start2=pos2;
		}
		//assign
		alignment.push_back(pair<int,int>(pos1-start1,pos2-start2));
		double signal=atof(out[4].c_str());
		raw_signal.push_back(signal);
		double expected=atof(out[3].c_str());
		expected_signal.push_back(expected);
		double sco1=atof(out[7].c_str());
		zsco_1.push_back(sco1);
		double sco2=atof(out[6].c_str());
		zsco_2.push_back(sco2);
		double diff=atof(out[9].c_str());
		zsco_diff.push_back(diff);
		//count++
		count++;
	}
	//return
	return count;
}


//-------- reformat signal -----------//
int Reformat_Signal(int sig_len,
	vector <pair<int,int> > &alignment, int FIRSTorSECOND,
	vector <double> &input_signal, vector <double> &output_signal, vector <int> &output_count,
	int &rel_start, int &rel_end)
{
	output_signal.resize(sig_len,0);
	output_count.resize(sig_len,0);
	//get start
	int start=-1;
	for(int i=0;i<(int)alignment.size()-1;i++)
	{
		int ii=alignment[i].first;
		int jj=alignment[i].second;
		int ii_1=alignment[i+1].first;
		int jj_1=alignment[i+1].second;
		if(ii+1==ii_1 && jj+1==jj_1) //start found
		{
			start=i;
			break;
		}
	}
	//get end
	int end=-1;
	for(int i=(int)alignment.size()-1;i>=1;i--)
	{
		int ii=alignment[i].first;
		int jj=alignment[i].second;
		int ii_1=alignment[i-1].first;
		int jj_1=alignment[i-1].second;
		if(ii-1==ii_1 && jj-1==jj_1) //start found
		{
			end=i;
			break;
		}
	}
	//check
	if(start==-1 || end==-1)
	{
		fprintf(stderr,"start %d or end %d bad !! \n",
			start,end);
		exit(-1);
	}
	//------ main process ------//
	vector <vector <double> > temp_rec;
	temp_rec.resize(sig_len);
	for(int i=start;i<=end;i++)
	{
		int ii=alignment[i].first;
		int jj=alignment[i].second;
		if(FIRSTorSECOND==0) //reformat first to second
		{
			temp_rec[jj].push_back(input_signal[ii]);
		}
		else                 //reformat second to first
		{
			temp_rec[ii].push_back(input_signal[jj]);
		}
	}
	//proc head
	{
		int ii=alignment[start].first;
		int jj=alignment[start].second;
		if(FIRSTorSECOND==0) //reformat first to second
		{
			for(int i=0;i<jj;i++)temp_rec[i].push_back(input_signal[ii]);
		}
		else                 //reformat second to first
		{
			for(int i=0;i<ii;i++)temp_rec[i].push_back(input_signal[jj]);
		}
	}
	//proc tail
	{
		int ii=alignment[end].first;
		int jj=alignment[end].second;
		if(FIRSTorSECOND==0) //reformat first to second
		{
			for(int i=jj+1;i<sig_len;i++)temp_rec[i].push_back(input_signal[ii]);
		}
		else                 //reformat second to first
		{
			for(int i=ii+1;i<sig_len;i++)temp_rec[i].push_back(input_signal[jj]);
		}
	}
	//---- finalize ------//
	for(int i=0;i<sig_len;i++)
	{
		double value=0;
		int count=0;
		for(int k=0;k<temp_rec[i].size();k++)
		{
			value+=temp_rec[i][k];
			count++;
		}
		if(count==0)
		{
			fprintf(stderr,"pos %d not covered !!! \n", i);
			exit(-1);
		}
		value=1.0*value/count;
		output_signal[i]=value;
		output_count[i]=count;
	}
	rel_start=start;
	rel_end=end;
	return end-start+1;
}


//---------- main ----------//
int main(int argc,char **argv)
{
	//---- AlignedSignal_Proc ----//
	{
		if(argc<6)
		{
			fprintf(stderr,"AlignedSignal_Proc <exp> <raw> <alignment> <first_or_second> <out_signal> \n");
			fprintf(stderr,"[note1]: <first_or_second> set 0 to transform raw to exp in length, \n");
			fprintf(stderr,"                           set 1 to transform exp to raw in length. \n");
			fprintf(stderr,"[note2]: <alignment> we ALWAYS put raw in the second. \n");
			exit(-1);
		}
		string in_exp=argv[1];
		string in_raw=argv[2];
		string align_file=argv[3];
		int first_or_second=atoi(argv[4]);
		string out_signal=argv[5];
		//-- load input_signal ---//
		vector <double> input_signal_exp;
		int sigexp_len=Load_Signal(in_exp,input_signal_exp);
		vector <double> input_signal_raw;
		int sigraw_len=Load_Signal(in_raw,input_signal_raw);
		//-- assign signal ---//
		vector <double> input_signal;
		int sig_len;
		if(first_or_second==0)  //-> transform raw to exp in length
		{
			input_signal=input_signal_raw;
			sig_len=(int)input_signal_exp.size();
		}
		else                    //-> transform exp to raw in length
		{
			input_signal=input_signal_exp;
			sig_len=(int)input_signal_raw.size();
		}
		//-- load alignment --//
		vector <pair<int,int> > alignment;
		vector <double> raw_signal;
		vector <double> expected_signal;
		vector <double> zsco_1;
		vector <double> zsco_2;
		vector <double> zsco_diff;
		int ali_size=Load_cwDTW_alignment_nostr(align_file,alignment,
			raw_signal,expected_signal,zsco_1,zsco_2,zsco_diff);
		//-- check length --//
		if(first_or_second==0)
		{
			if(sig_len<alignment[alignment.size()-1].second+1)
			{
				fprintf(stderr,"sig_len %d less than exp_len %d \n",
					sig_len,alignment[alignment.size()-1].second+1);
				exit(-1);
			}
		}
		else
		{
			if(sig_len<alignment[alignment.size()-1].first+1)
			{
				fprintf(stderr,"sig_len %d less than raw_len %d \n",
					sig_len,alignment[alignment.size()-1].first+1);
				exit(-1);
			}
		}
		//-- reformat sigA to sigB ---//
		vector <double> output_signal;
		vector <int> output_count;
		int rel_start;
		int rel_end;
		int rel_len=Reformat_Signal(sig_len,alignment,first_or_second,
			input_signal,output_signal,output_count,rel_start,rel_end);
		//--- output to file --//
		FILE *fp=fopen(out_signal.c_str(),"wb");
		for(int i=0;i<(int)output_signal.size();i++)fprintf(fp,"%lf %d\n",output_signal[i],output_count[i]);
		fclose(fp);
		//--- screen output ---//
		printf("rel_start: %8d | rel_end: %8d | rel_len: %8d | sig_len: %8d \n",
			rel_start,rel_end,rel_len,sig_len);
		//--- exit ---//
		exit(0);
	}
}

