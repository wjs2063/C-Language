
#define _CRT_NO_SECURE_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

unsigned int i_total,i_miss,d_total, d_miss, d_write;
unsigned int intotal, inmiss;
unsigned int time_count;
unsigned int dirtytotal = 0;

void fetch(int address, int csize, int bszie, int associ);
void read( int address, int csize, int bsize, int associ);
void write(int address, int csize, int bsize, int associ);
void simulate( int csize, int bsize, int associ);
int vict(int set, int associ, char mode);
#define file "sample.txt"

typedef struct Cache {
	unsigned int tag;
	unsigned int valid;
	unsigned int time;
	int dirty;

}Dcache;
Dcache *dc;
typedef struct Icache {
	int tag;
	int valid;
	int time;
}Icache;
Icache *ic;

int main(int argc,char * argv[]) {
	 unsigned int s,b,a,address;//s =cache size �� b=block size a �� associative
	
	 
	printf("csize�� bsize �� associ�� �Է��ϼ���\n");
	scanf_s("%d %d %d", &s, &b, &a);
	//�������� csize �� bsize �� 2�� �����¹��̸� associ �� 1 2 4 8�� �ϳ��̴� Byte �����̹Ƿ�
	s = s*pow(2, 8);
	b = b*pow(2, 8);
	
	simulate(s, b, a);
	return 0;
}






void simulate(int csize, int bsize, int associ) {
	i_total = 0, i_miss = 0, d_total = d_miss = d_write = 0;
	double dmissrate,imissrate;
	intotal = inmiss = 0;
	 unsigned int address,data;
	 char mode;
	unsigned int num = csize / bsize;
	FILE *fp = NULL;
	dc = (Dcache*)calloc(num, sizeof(Dcache));
	
	ic = (Icache*)calloc(num, sizeof(Icache));
	fp = fopen(file, "r");
	while (!feof(fp)) {
		fscanf(fp, "%x %c\n", &address, &mode);
		if (mode == 'W')
			fscanf(fp, "%d", &data);
		if (mode == 'R') {
			read(address, csize, bsize, associ);
			d_total++; 
			
		}
		else if (mode == 'W') {
			write(address, csize, bsize, associ);
			d_total++;
		}
		else {
			fetch(address, csize, bsize, associ);
			i_total++;


		}
		time_count++;
		printf(" %.8x  v: %d d:  %d\n", address, dc->valid, dc->dirty);
	}

	free(dc);
	dmissrate = (double)d_miss / (double)d_total;
	imissrate = (double)i_miss / (double)i_total;
	double totalmissrate = (double)(i_miss + d_miss) /(double) (d_total + i_total);
	printf("total number of hits:  %d\n", i_total + d_total - i_miss-d_miss);
	printf("total number of misses:  %d\n", i_miss + d_miss);
	printf("miss rate %lf%%\n", totalmissrate*100);
	printf("total number of dirty blocks= %d \n", dirtytotal);
	int hittime = i_total + d_total - i_miss - d_miss;
	double access =(double) hittime + totalmissrate * 200;
	printf("Average access cycle %.lf \n", access);
	
	
}

void read(int address, int csize, int bsize, int associ) {
	unsigned int setnumber, set;
	unsigned int i, victim, avail = 10;
	 Dcache *p=dc;
	setnumber = csize / (bsize*associ);
	set = (address / bsize) % setnumber;
	for (i = 0; i < associ; i++) {
		p = &dc[set*associ + i];
		if (p->valid == 1 && p->tag == ((address / bsize) / setnumber)) {
			p->time = time_count;
			return;
		}
		else if (p->valid == 0) {
			if (i < avail) avail = i;
		}
	
	}

	d_miss++;
	if (avail == 10){
		victim = vict(set, associ, 'd');
	p = &dc[set*associ + victim];
	if (p->dirty) d_write++;
	p->valid = 1;
	p->time = time_count;
	p->tag = (address / bsize) / setnumber;
	p->dirty = 0;//new block dirty is 0 �̴�
	
}
else {
	// ĳ���� �ڸ��������� 
	p = &dc[set*associ + avail];
	p->valid = 1;
	p->time = time_count;
	p->tag = (address / bsize) / setnumber;
	p->dirty = 0;

}
}
void fetch( int address, int csize,  int bsize, int associ) {
	int setnumber, set;
 int i, victim, avail = 10;
	Icache *p;
	setnumber = csize / (bsize*associ);
	set = (address / bsize) % setnumber;
	for (i = 0; i < associ; i++) {
		p = &ic[set*associ + i];
		if (p->valid == 1 && p->tag == (address / bsize) / setnumber) {
			p->time = time_count;
			return;
		}
		else if (p->valid == 0) {
			if (i < avail)avail = i;
		}
	}
	i_miss++;
	if (avail == 10) {
		victim = vict(set, associ, 'W');
		p = &ic[set*associ + victim];
		p->valid = 1;
		p->time = time_count;
		p->tag = (address / bsize) / setnumber;
	}
	else { p = &ic[set*associ + avail];
	p->valid = 1;
	p->time = time_count;
	p->tag = (address / bsize) / setnumber;
	}
}

int vict(int set, int associ, char mode) {
	unsigned int i, temp_time = 0;
	unsigned  int min = time_count + 1, min_k = 0;
	for (i = 0; i < associ; i++) {
		if(mode=='R')temp_time = dc[set*associ + i].time;
		if (mode == 'W')temp_time = ic[set*associ + i].time;
		if (min > temp_time) {
			min = temp_time;
			min_k = i;
		}
	}
	return min_k;
}


void write(int address, int csize, int bsize, int associ) {
	 int setnumber, set;
	int  i, victim = 0 ,avail=10;
	Dcache *p=dc;
	setnumber = csize / (bsize*associ);
	set = (address / bsize) % setnumber;

	//hit �� miss �Ǵ��غ���

	for (i = 0; i < associ; i++) {
		p = &dc[set*associ + i];
		if (p->valid == 1 && p->tag == (address / bsize) / setnumber) {
			p->time = time_count;
			p->dirty = 1;
			dirtytotal++;
			
			return;
		}
		else if (p->valid == 0) {
			if (i < avail) avail = i;
			
		}
	}
	//�̱����Դٸ� �����̾��� �� MISS ++���ְ� new block �� �÷�����
	d_miss++;
	if (avail == 10) {
		victim = vict(set, associ, 'd');
		p = &dc[set*associ + victim];
		if (p->dirty)d_write++;
		p->valid = 1;
		p->time = time_count;
		p->tag = (address / bsize) / setnumber;
		p->dirty = 1;
		dirtytotal++;
	}
	else {
		p = &dc[set*associ + avail];
		p->valid = 1;
		p->time = time_count;
		p->tag = (address / bsize) / setnumber;
		p->dirty = 1;
		dirtytotal++;
	}
}