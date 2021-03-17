#include <stdio.h>
//비교연산과 교환횟수를 출력하는 program작성

void swap(int arr[],int i,int j){
    int t=arr[i];
    arr[i]=arr[j];
    arr[j]=t;
}
void bubblesort(int arr[],int n,int *count,int *relate){
    int i,j;
    // 오름차순 정렬
    for(i=0;i<n-1;i++)
        for(j=n-1;j>i;j--){
            (*relate)++;
            if(arr[j-1]>arr[j]){
                swap(arr,j,j-1);
                (*count)++;// 교환수행 count
                }
            
        }


    
    
}




int main(){
int arr[7]={6,4,3,7,1,9,8};
int num1=0;int num2=0;
int *count=&num1;int *relate=&num2;
printf("정렬전 요소들의값은:\n");
for(int i=0;i<7;i++){
    printf("arr[%d]=%d\n",i,arr[i]);
}
bubblesort(arr,7,count,relate);
printf("정렬후 요소들의 값은:\n");
for(int i=0;i<7;i++){
    printf("arr[%d]=%d\n",i,arr[i]);
}
printf("비교횟수는:%d, 교환횟수는:%d",*relate,*count);

}