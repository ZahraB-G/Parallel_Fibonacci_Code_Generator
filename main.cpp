#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <string>
#include<iostream>
#include <vector>
#include <fstream>
#include <algorithm>

using namespace std;
struct node {
    string fileInput;
    char symbol;
    int frequency;
    int assignedInt;
    string fibcode;
    int fibIndex[4];
    node() {};
    node(string fs) { fileInput = fs; }
};
void* inc_x(void* x_void_ptr)
{
    node* x_ptr = (node*)x_void_ptr;
    for (int i = 0; i < (*x_ptr).fileInput.length(); i++)
        if ((*x_ptr).symbol == (*x_ptr).fileInput[i])
        {
            (*x_ptr).frequency++;
        }
    return NULL;
}
//This function calculating frequency by using multithread 
//this struct is used to in sort based on frequency.
struct greater_than_key
{
    inline bool operator() (const node& node1, const node& node2)
    {
        if (node1.frequency == node2.frequency)
            return(node1.symbol > node2.symbol);
        return (node1.frequency > node2.frequency);
    }
};
struct greater_than_alphebet
{
    inline bool operator() (const node& node1, const node& node2)
    {
        return (node1.symbol < node2.symbol);
    }
};
/*This Function recursively calculate fibonacci number*/
int RecursiveFibFunction(int n)
{
    if (n <= 1)
        return n;
    else
        return RecursiveFibFunction(n - 2) + RecursiveFibFunction(n - 1);
}
/*this function fill out the fibonacci table*/
void FillFibTable(int* fibTable)
{
    for (int i = 1; i < 20; i++)
    {
        fibTable[i] = RecursiveFibFunction(i);
    }
}
void* fib(void* x_void_ptr)
{
    node* x_ptr = (node*)x_void_ptr;
    string result;
    if ((*x_ptr).fibIndex[0] == 1)
    {
        (*x_ptr).fibcode = "11";
        return NULL;
    }
    int x = (*x_ptr).fibIndex[0] - 2;
    for (int j = 0; j <= x; j++)
    {
        result += "0";
    }
    result[x] = '1';
    int z = 1;
    while ((*x_ptr).fibIndex[z] != 0)
    {
        x = (*x_ptr).fibIndex[z] - 2;
        if (x < -1)
            break;
        if (x == -1)
        {
            result[0] = '1';
            break;
        }
        if (result[x + 1] != '1' && result[x - 1] != '1')
        {
            result[x] = '1';
        }
        z++;
    }
    (*x_ptr).fibcode = result + "1";
    return NULL;
}
int FindClosesFibNum(int n, int* fibTable)
{
    int i = 0;
    while (n >= fibTable[i])
        i++;
    return fibTable[i - 1];
}
int FindFibIndex(int n, int* fibTable)
{
    int i = 1;
    while (n != fibTable[i])
    {
        i++;
    }
    return i;
}
int main()
{

    //this is first step to get the inputfile and originalcharacters
    static string fileInput; //this variable contains the file's string which is accessible everywhere
    static int* fibTable = new int[20];
    static string originalString; //originalstring which is gonna be repeated in file
    string fileName; //name of the file that I have to read
    getline(cin, originalString);
    getline(cin, fileName);
    ifstream inputFile;
    inputFile.open(fileName.c_str());
    if (inputFile.is_open())
        inputFile >> fileInput;
    inputFile.close();

    int originalStringLength = originalString.length();
    static vector<node> initialVector;
    //this loop reads originalstring and push it to the vector 
    for (int i = 0; i < originalString.length(); i++)
    {
        initialVector.push_back(node(fileInput));
        initialVector[i].symbol = originalString[i];
    }
    //Calculating frequency based by using multithreading
    pthread_t* tid;
    tid = new pthread_t[originalStringLength];
    for (int i = 0; i < originalStringLength; i++)
    {
        if (pthread_create(&tid[i], NULL, inc_x, &initialVector[i]))
        {
            fprintf(stderr, "Error creating thread\n");
            return 1;

        }

    }
    //waiting for child thread
    for (int i = 0; i < originalStringLength; i++)
    {
        pthread_join(tid[i], NULL);
    }
    //this sort is for step 4 before assigning the integer to each symbol based on frequency
    sort(initialVector.begin(), initialVector.end(), greater_than_key());

    int counter = 1;
    //Assigning integer to each symbol based on their frequency
    for (int i = 0; i <= originalString.length(); i++)
    {
        initialVector[i].assignedInt = counter++;
    }

    FillFibTable(fibTable);
    int reminder;
    int j, n, x;
    int fibnumber;
    //This loop assign fibIndexes for each node
    for (int i = 0; i <= originalString.length(); i++)
    {
        reminder = initialVector[i].assignedInt;
        x = 0;
        j = 0;
        int n = initialVector[i].assignedInt;
        while (reminder != 0)
        {
            fibnumber = FindClosesFibNum(n, fibTable);
            reminder = n - fibnumber;
            initialVector[i].fibIndex[j++] = FindFibIndex(fibnumber, fibTable);
            n = reminder;
        }
    }

    //Calculating fibbonacci code word based by using multithreading
    for (int i = 0; i < originalStringLength; i++)
    {
        if (pthread_create(&tid[i], NULL, fib, &initialVector[i]))
        {
            fprintf(stderr, "Error creating thread\n");
            return 1;

        }

    }
    //waiting for child thread
    for (int i = 0; i < originalStringLength; i++)
    {
        pthread_join(tid[i], NULL);
    }

    //This is step 5 which is the last sort function based on alphabets .
    sort(initialVector.begin(), initialVector.end(), greater_than_alphebet());
    //output of my project format.
    for (int i = 0; i < initialVector.size(); i++)
        cout << "Symbol: " << initialVector[i].symbol << ", Frequency: " << initialVector[i].frequency << ", Code: " << initialVector[i].fibcode << endl;
    delete[] tid;
    delete[] fibTable;
    return 0;

}
