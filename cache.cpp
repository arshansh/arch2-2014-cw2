/*

 * cache.cpp
 *
 *  Created on: Dec 3, 2014
 *      Author: sh_arsh2000
 */
#include <iostream>
#include<stdio.h>
#include <cstdlib>
#include<string>
#include<cmath>
#include<vector>
#include<sstream>
#include"stdint.h"
#include <cstdio>


using namespace std;



struct cache_word{
	vector<int> a1;

};
struct  cache_block{
	//int byte_address;	//address of the first word the block holds
	int  lru=0;
	bool dirty_flag=0;
	int tag;
	bool valid_bit=0;
	vector <cache_word> a2;
};
struct cache_set{
	vector <cache_block> a3;
};
struct cache{
	vector <cache_set> a4;
};

struct memory{
	vector<cache_word> a5;
};



bool cache_hit_miss(int byte_address,cache a,int & block_offset,cache_word &word);
void read_request(int byte_address,cache &a,memory &mem,cache_word &word,int hit_time,int read_time,int write_time);
void write_cash(int byte_address,cache &cache,memory &mem,int &block_offset,cache_word &word,int &time,int read_time,int write_time);
void write_request(int byte_address,cache &a,memory &mem,cache_word word,int hit_time,int read_time,int write_time);
void flush_request(cache &a,memory &mem,cache_word word,int write_time);




int main(int argc,char* argv[]){



unsigned  int address_bits=atoi(argv[1]);	//atoi function converts a string to an int
unsigned  int bytes_word=atoi(argv[2]);
unsigned  int word_block=atoi(argv[3]);
unsigned  int block_set=atoi(argv[4]);
unsigned  int set_cache=atoi(argv[5]);
unsigned  int hit_time=atoi(argv[6]);
unsigned  int read_time=atoi(argv[7]);
unsigned  int write_time=atoi(argv[8]);




cache cache;
memory mem;

	cache.a4.resize(set_cache);
	for(int i=0;i<set_cache;i++){
		cache.a4[i].a3.resize(block_set);
			for(int j=0;j<block_set;j++){
				cache.a4[i].a3[j].a2.resize(word_block);
				for(int k=0;k<word_block;k++){
					cache.a4[i].a3[j].a2[k].a1.resize(bytes_word);
				}
			}
	}

	mem.a5.resize(1000);
	for(int i=0;i<1000;i++){
	mem.a5[i].a1.resize(bytes_word);
	}



string cmd;

while(cin>>cmd && !cin.eof()){	//while reading from the stdin




	if(cmd=="read-req"){

		cache_word tmp;
		tmp.a1.resize(bytes_word);

		unsigned int  byte_addr;
		cin>>byte_addr;
		read_request(byte_addr,cache,mem,tmp,hit_time,read_time,write_time);
	}

	else if(cmd=="write-req"){
		unsigned int byte_addr;
		string data;
		cin>>byte_addr;
		cin>>data;	//change data to word

		cache_word temp_word;	//creating a temp word which holds the value in memory


		for(int k=0;k<bytes_word;k++){
		string substring=data.substr(2*k,2);		//getting the string 2 by 2

		int hex_value = strtoul(substring.c_str(), NULL, 16);
		//cout<<hex<<hex_value<<endl;
		temp_word.a1.push_back(hex_value);
		}
		//cout<<hex<<temp_word.a1[0]<<temp_word.a1[1]<<endl;

		write_request(byte_addr,cache,mem,temp_word,hit_time,read_time,write_time);

	}
	else if(cmd=="flush-req"){

		cache_word tmp_word;
		tmp_word.a1.resize(bytes_word);

		flush_request(cache,mem,tmp_word,write_time);

	}
	else if(cmd=="debug-req"){		//printing the whole cache

		cout<<"debug-ack-begin"<<endl;

		for(int i=0;i<set_cache;i++){
			cout<<endl;
			for(int j=0;j<block_set;j++){
				cout<<"          ";
				for(int k=0;k<word_block;k++){
					cout<<"  ";
					for(int z=0;z<bytes_word;z++){
						cout<<hex<<cache.a4[i].a3[j].a2[k].a1[z];
					}
				}
			}
		}

		cout<<endl;
		cout<<"debug-ack-end"<<endl;

	}
	else if(cmd=="#"){
		//followed by  any characters ??
	}



}


return 0;

}


bool cache_hit_miss(int byte_address,cache a,int & block_offset,cache_word &word){

	int bytes_word,word_block,block_set,set_cache;

	set_cache=a.a4.capacity();
	block_set=a.a4[0].a3.capacity();
	word_block=a.a4[0].a3[0].a2.capacity();
	bytes_word=a.a4[0].a3[0].a2[0].a1.capacity();




	//calculating word_offset
	int word_offset=(byte_address/bytes_word)% word_block;	//notice word per block excludes the number of blocks in each set if more than 1

	//calculating tag
	int no_byteoffset_bits=log2(bytes_word);
	int no_wordoffset_bits=log2(word_block);
	int no_set_index_bits=log2(set_cache);


	int tag=byte_address >> (no_byteoffset_bits + no_wordoffset_bits + no_set_index_bits);


	//calculating the set_index
	int bytes_block=bytes_word*word_block;
	int block_address=byte_address/bytes_block;
	int set_index=block_address%set_cache;



			for(int j=0;j<block_set;j++){
				if(a.a4[set_index].a3[j].tag==tag){
					block_offset=j;		//block_offset will only be updated if its a hit
					if(a.a4[set_index].a3[j].valid_bit==1){
						word=a.a4[set_index].a3[j].a2[word_offset];		//word will only be updated if its a hit
							return true;
					}
				}
			}

	return false;
}







void read_request(int byte_address,cache &a,memory &mem,cache_word &word,int hit_time,int read_time,int write_time){

	int bytes_word,word_block,block_set,set_cache;

	set_cache=a.a4.capacity();
	block_set=a.a4[0].a3.capacity();
	word_block=a.a4[0].a3[0].a2.capacity();
	bytes_word=a.a4[0].a3[0].a2[0].a1.capacity();




	//calculating word_offset
	int word_offset=(byte_address/bytes_word)% word_block;	//notice word per block excludes the number of blocks in each set if more than 1

	//calculating tag
	int no_byteoffset_bits=log2(bytes_word);
	int no_wordoffset_bits=log2(word_block);
	int no_set_index_bits=log2(set_cache);

	int tag=byte_address >> (no_byteoffset_bits + no_wordoffset_bits + no_set_index_bits);


	//calculating the set_index
	int bytes_block=bytes_word*word_block;
	int block_address=byte_address/bytes_block;
	int set_index=block_address%set_cache;

	int time=hit_time;		// if its a hit

	bool hit_or_miss;
	int block_offset=-1;		//either gets updated on a hit or a miss
	hit_or_miss=cache_hit_miss(byte_address,a,block_offset,word);					//check if the address in in cache or not

	if(hit_or_miss==true){		//notice block_offset and word have been updated correctly

		for(int j=0;j<block_set;j++){
			if(a.a4[set_index].a3[j].valid_bit==1){
				a.a4[set_index].a3[j].lru++;			//each time we read hit we increment the lru of all blocks by 1
			}
		}
		a.a4[set_index].a3[block_offset].lru=0;		//when ever we get a hit we replace the lru of the specified block with zero



		cout<<"read-ack"<<" ";

		for(int k=0;k<bytes_word;k++){		//printing the word
			cout<<hex<<word.a1[k];
		}

		cout<<"  "<<set_index<< " hit "<<time<<endl;
	}



	else{		// if its a miss

		//read from memory and write to cache all the memory words needed
		write_cash(byte_address,a,mem,block_offset,word,time,read_time,write_time);
		a.a4[set_index].a3[block_offset].dirty_flag=0;		//dirty flag always goes to zero in read miss

		cout<<"read-ack"<<" ";

		for(int k=0;k<bytes_word;k++){		//printing the word
			cout<<hex<<word.a1[k];
		}

		cout<<"  "<<set_index<<" miss "<<time<<endl;
	}


}

void write_request(int byte_address,cache &a,memory &mem,cache_word word,int hit_time,int read_time,int write_time){

	int bytes_word,word_block,block_set,set_cache;

	set_cache=a.a4.capacity();
	block_set=a.a4[0].a3.capacity();
	word_block=a.a4[0].a3[0].a2.capacity();
	bytes_word=a.a4[0].a3[0].a2[0].a1.capacity();




	//calculating word_offset
	int word_offset=(byte_address/bytes_word)% word_block;	//notice word per block excludes the number of blocks in each set if more than 1

	//calculating tag
	int no_byteoffset_bits=log2(bytes_word);
	int no_wordoffset_bits=log2(word_block);
	int no_set_index_bits=log2(set_cache);


	int tag=byte_address >> (no_byteoffset_bits + no_wordoffset_bits + no_set_index_bits);


	//calculating the set_index
	int bytes_block=bytes_word*word_block;
	int block_address=byte_address/bytes_block;
	int set_index=block_address%set_cache;

	int time=hit_time;		// if its a hit

	bool hit_or_miss;
	int block_offset=-1;		//either gets updated on a hit or a miss
	cache_word mem_value;	//we dont care about the current memory value
	hit_or_miss=cache_hit_miss(byte_address,a,block_offset,mem_value);					//check if the address in in cache or not



	if(hit_or_miss==true){			//if its a hit

		for(int j=0;j<block_set;j++){
			if(a.a4[set_index].a3[j].valid_bit==1){
				a.a4[set_index].a3[j].lru++;			//each time we write hit we increment the lru of all blocks by 1
			}
		}
		a.a4[set_index].a3[block_offset].a2[word_offset]=word;	//rewriting the cache with the word
		a.a4[set_index].a3[block_offset].dirty_flag=1;	//memory and cache do not match
		a.a4[set_index].a3[block_offset].lru=0;		//when ever we get a hit we replace the lru of the specified block with zero

		cout<<"write-ack"<<" ";

		cout<<set_index<< "  hit  "<<time<<endl;
	}

	else{	// if its a miss

		//read from memory and write to cache all the memory words needed
		cache_word mem_value;	//we dont care about the current memory value
		write_cash(byte_address,a,mem,block_offset,mem_value,time,read_time,write_time);

		a.a4[set_index].a3[block_offset].a2[word_offset]=word;
		a.a4[set_index].a3[block_offset].dirty_flag=1;	//setting the dirty flag as 1
		a.a4[set_index].a3[block_offset].lru=0;

		cout<<"write-ack"<<"  ";

		cout<<set_index<< "  miss  "<<time<<endl;

	}

}
void write_cash(int byte_address,cache &a,memory &mem,int &block_offset,cache_word &word,int &time,int read_time,int write_time){

	int bytes_word,word_block,block_set,set_cache;

	set_cache=a.a4.capacity();
	block_set=a.a4[0].a3.capacity();
	word_block=a.a4[0].a3[0].a2.capacity();
	bytes_word=a.a4[0].a3[0].a2[0].a1.capacity();



	//calculating word_offset
	int word_offset=(byte_address/bytes_word)% word_block;	//notice word per block excludes the number of blocks in each set if more than 1


	//calculating tag
	int no_byteoffset_bits=log2(bytes_word);
	int no_wordoffset_bits=log2(word_block);
	int no_set_index_bits=log2(set_cache);


	int tag=byte_address>> (no_byteoffset_bits + no_wordoffset_bits + no_set_index_bits);


	//calculating the set_index
	int bytes_block=bytes_word*word_block;

	int block_address=byte_address/bytes_block;
	int set_index=block_address%set_cache;
	int byte_offset=byte_address%bytes_word;

	word=mem.a5[byte_address/bytes_word];		//updating data with the value in memory


		for(int j=0;j<block_set;j++){
			if(a.a4[set_index].a3[j].valid_bit==1){
				a.a4[set_index].a3[j].lru++;			//each time we write we increment the lru of all block by 1
			}
		}



		for(int j=0;j<block_set;j++){				//checking if any of the blocks within a set is empty and write the block in the first empty place

			if(a.a4[set_index].a3[j].valid_bit==0){
				block_offset=j;

				int last_byte_address=byte_address-(byte_address%bytes_block);		//copying all the words needed
				for(int k=last_byte_address;k<last_byte_address+bytes_block;k++){

					int temp_word_offset=(k/bytes_word)% word_block;		//calculating the word offset for each block
					int temp_byte_offset=k%bytes_word;			//calculating the byte offset for each word
					//copying all the words within a block

					int temp;
					temp=mem.a5[k/bytes_word].a1[temp_byte_offset];

					a.a4[set_index].a3[j].a2[temp_word_offset].a1[temp_byte_offset]=temp;
				}

					a.a4[set_index].a3[j].tag=tag;
					a.a4[set_index].a3[j].dirty_flag=0;		//memory and cache match now
					a.a4[set_index].a3[j].valid_bit=1;
					a.a4[set_index].a3[j].lru=0;
					time+=read_time;	//we just read from memory

					return;
			}
		}


			//if all the blocks within a set are full(valid),replace it with the LRU one
			int min_used_block = 0;
			int max_lru=a.a4[set_index].a3[0].lru;

			for(int j=0;j<block_set;j++){
				if(a.a4[set_index].a3[j].lru>max_lru){		//checking for the max lru(one which has been in the cache longer and has not been read or written)
					max_lru=a.a4[set_index].a3[j].lru;
					min_used_block=j;
				}
			}
			block_offset=min_used_block;

			bool dirty_flag=0;		//bool to check if we have replaced(useful for timing)

			//copying the block and writing it to memory
			if(a.a4[set_index].a3[min_used_block].dirty_flag==1){		//if the block we are rewriting is not consistent with the memory,we must first write it to memory



				//write mem function

				int first_tag=a.a4[set_index].a3[min_used_block].tag;

				int tag_shifting=no_byteoffset_bits+no_wordoffset_bits+no_set_index_bits;
				int index_shifting=no_byteoffset_bits+no_wordoffset_bits;
				//calculating the byte address for the first byte of the first word of that block
				int first_byte_address=(first_tag<< tag_shifting)+(set_index<<index_shifting);

				for(int k=first_byte_address;k<first_byte_address+bytes_block;k++){


					int temp_word_offset=(k/bytes_word)% word_block;		//calculating the word offset for the block
					int temp_byte_offset=k%bytes_word;

					//copying all the words within a block
					int temp;
					temp=a.a4[set_index].a3[min_used_block].a2[temp_word_offset].a1[temp_byte_offset];		//byte offset is the same in cache and memory

					mem.a5[k/bytes_word].a1[temp_byte_offset]=temp;


				}
				dirty_flag=1;
				time+=read_time+write_time;		//reading and writing to memory
			}





			int last_byte_address=byte_address-(byte_address%bytes_block);		//copying all the words needed
			for(int k=last_byte_address;k<last_byte_address+bytes_block;k++){

				int temp_word_offset=(k/bytes_word)% word_block;		//calculating the word offset for the block
				int temp_byte_offset=k%bytes_word;
				//copying all the words within a block
				int temp;
				temp=mem.a5[k/bytes_word].a1[temp_byte_offset];
				a.a4[set_index].a3[min_used_block].a2[temp_word_offset].a1[temp_byte_offset]=temp;
			}
			a.a4[set_index].a3[min_used_block].tag=tag;
			a.a4[set_index].a3[min_used_block].dirty_flag=0;		//memory and cache match now
			a.a4[set_index].a3[min_used_block].valid_bit=1;
			a.a4[set_index].a3[min_used_block].lru=0;
			if(dirty_flag==0){
				time+=read_time;
			}
			return;


}



void flush_request(cache &a,memory &mem,cache_word word,int write_time){

	int bytes_word,word_block,block_set,set_cache;

	set_cache=a.a4.capacity();
	block_set=a.a4[0].a3.capacity();
	word_block=a.a4[0].a3[0].a2.capacity();
	bytes_word=a.a4[0].a3[0].a2[0].a1.capacity();



	//calculating tag
	int no_byteoffset_bits=log2(bytes_word);
	int no_wordoffset_bits=log2(word_block);
	int no_set_index_bits=log2(set_cache);


	int bytes_block=bytes_word*word_block;
	int time=0;

	for(int i=0;i<set_cache;i++){
		for(int j=0;j<block_set;j++){

			if(a.a4[i].a3[j].valid_bit==1 && a.a4[i].a3[j].dirty_flag==1){



				//write mem function

				int first_tag=a.a4[i].a3[j].tag;

				int tag_shifting=no_byteoffset_bits+no_wordoffset_bits+no_set_index_bits;
				int index_shifting=no_byteoffset_bits+no_wordoffset_bits;
				//calculating the byte address for the first byte of the first word of that block
				int first_byte_address=(first_tag<< tag_shifting)+(i<<index_shifting);

				for(int k=first_byte_address;k<first_byte_address+bytes_block;k++){


					int temp_word_offset=(k/bytes_word)% word_block;		//calculating the word offset for the block
					int temp_byte_offset=k%bytes_word;

					//copying all the words within a block
					int temp;
					temp=a.a4[i].a3[j].a2[temp_word_offset].a1[temp_byte_offset];		//byte offset is the same in cache and memory
					mem.a5[k/bytes_word].a1[temp_byte_offset]=temp;

				}

			a.a4[i].a3[j].dirty_flag=0;		//setting the dirty flag to 0
			time+=write_time;
			}

		}
	}

	cout<<"flush-ack"<<" ";
	cout<<time<<endl;
}


