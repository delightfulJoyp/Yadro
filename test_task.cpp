// test_task.cpp

#include <iostream>
#include <fstream>
#include <string>

using namespace std;

class Tape {
private:
    string tapename;
    ifstream fin;
    ofstream fout;
    int usedTime;
public:
    int getUsedTime() {  // returns the time value (in milliseconds) that was spent by a particular tape
        return usedTime; // (whether input, buffer, or output) during its entire running time
    }

    void setname(string name) { // sets the name (path) for one of the tapes
        tapename = name;
    }

    string getname() { // returns the name(path) of one of the tapes
        return tapename;
    }

    void openOutput(int tapeRewind_delay) {         // opens the output tape with specified values for such an object,
        fout.open(tapename, ios::out | ios::trunc); // such as "clear file before reuse", as well as counts the time spent on one rewind of the tape
        cout << "File " << tapename << " is open successfully !\n";
        usedTime += tapeRewind_delay;
    }

    bool openInput(int tapeRewind_delay) { // opens the input tape with the specified values for such an object,
        fin.open(tapename, ios::in);       // counts the time it takes to rewind the tape and checks if the file is open,
        if (!fin.is_open()) {              // otherwise it stops the program and displays an error message
            cout << "Can't open the " << tapename << " file, please create .txt file and restart the program !\n";
            while (true);
            return false;
        }
        cout << "File " << tapename << " is open successfully !\n";
        usedTime += tapeRewind_delay;
        return true;
    }

    void close() { // closes any tape, whether it's the input or output
        fout.close();
        fin.close();
        cout << "File " << tapename << " is closed successfully !\n";
    }

    void setData(unsigned int data, int readWrite_delay, int tapeMove_delay) { // row by row sets the values received from the previous tape, 
        usedTime += readWrite_delay;                                           // as well as counts the time it took to write and move the tape to the next "line" of output tape
        usedTime += tapeMove_delay;
        fout << data << endl;
    }

    unsigned int getData(int readWrite_delay, int tapeMove_delay) { // gets a value from the input tape (all three tapes become input at some point),
        unsigned int data;                                          // also counts the time it takes to read and move the tape to the next value
        fin >> data;
        usedTime += readWrite_delay;
        usedTime += tapeMove_delay;
        return data;
    }

    bool is_end() { // analog of the method from the ifstream class, it checks if the file has ended, otherwise it returns zero
        if (fin.eof()) { return 1; }
        else { return 0; }
    }
};
Tape tout; // Initializing objects of the Tape class for subsequent work with these objects in the Sorting class
Tape tin;
Tape tbuf;
class Sorting {
private:
    string name;
    ifstream cfg;
    int readWrite_delay;
    int tapeRewind_delay;
    int tapeMove_delay;
    unsigned int prevData;
    unsigned int currData;
    bool checker;
    int totalTime;

    // the following three class methods (in2buff, out2buff, buff2out) work according to the following algorithm: opening two "tapes" in the desired mode (input or output),
    // getting a "string" of information from the input tape, comparing the information with the previous string and,
    // if data are in ascending order, it writes a smaller number to the output file, while the larger number is stored in the computer memory, then both "tapes" are closed.

    void in2buff() {                        // the method performs a primary transfer of data from the input tape to the buffer tape with primary sorting. 
        tin.openInput(tapeRewind_delay);    // after reading the information from the input tape, it is no longer used.
        tbuf.openOutput(tapeRewind_delay);

        prevData = tin.getData(readWrite_delay, tapeMove_delay);
        cout << prevData << endl;
        for (currData = tin.getData(readWrite_delay, tapeMove_delay); !tin.is_end(); currData = tin.getData(readWrite_delay, tapeMove_delay)) {
            cout << currData << endl;
            if (prevData > currData) {
                tbuf.setData(currData, readWrite_delay, tapeMove_delay);
            }
            else {
                tbuf.setData(prevData, readWrite_delay, tapeMove_delay);
                prevData = currData;
            }
        }
        tbuf.setData(prevData, readWrite_delay, tapeMove_delay);

        tin.close();
        tbuf.close();
    }

    void out2buff() {                     // the method transfers the information from the output tape to the buffer tape, sorting the data in parallel. 
        tout.openInput(tapeRewind_delay); // each run of this function initializes just one pass of the sorting algorithm through all the data
        tbuf.openOutput(tapeRewind_delay);

        prevData = tout.getData(readWrite_delay, tapeMove_delay);
        cout << prevData << endl;
        for (currData = tout.getData(readWrite_delay, tapeMove_delay); !tout.is_end(); currData = tout.getData(readWrite_delay, tapeMove_delay)) {
            cout << currData << endl;
            if (prevData > currData) {
                tbuf.setData(currData, readWrite_delay, tapeMove_delay);
            }
            else {
                tbuf.setData(prevData, readWrite_delay, tapeMove_delay);
                prevData = currData;
            }
        }
        tbuf.setData(prevData, readWrite_delay, tapeMove_delay);

        tout.close();
        tbuf.close();
    }

    bool buff2out() {                     // the method transfers the information from the buffer tape to the output tape, sorting the data in parallel. 
        tbuf.openInput(tapeRewind_delay); // each run of this function initializes just one pass of the sorting algorithm through all the data
        tout.openOutput(tapeRewind_delay);

        checker = true;
        prevData = tbuf.getData(readWrite_delay, tapeMove_delay);
        cout << prevData << endl;
        for (currData = tbuf.getData(readWrite_delay, tapeMove_delay); !tbuf.is_end(); currData = tbuf.getData(readWrite_delay, tapeMove_delay)) {
            cout << currData << endl;
            if (prevData > currData) {
                tout.setData(currData, readWrite_delay, tapeMove_delay);
                checker = false;
            }
            else {
                tout.setData(prevData, readWrite_delay, tapeMove_delay);
                prevData = currData;
            }
        }
        tout.setData(prevData, readWrite_delay, tapeMove_delay);

        tbuf.close();
        tout.close();

        return checker;
    }
public:
    void init() { // To initialize the sorting algorithm program needs to get the names (paths) of input and output files, as well as reading information from the configuration file.
        cout << "Enter input file name : ";
        cin >> name;
        tin.setname(name);
        cout << "Enter output file name : ";
        cin >> name;
        tout.setname(name);
        tbuf.setname("tmp/buffer.txt");
        cfg.open("cfg.txt", ios::in);
        cfg >> readWrite_delay;
        cfg >> tapeRewind_delay;
        cfg >> tapeMove_delay;
        cfg.close();
        cout << "Configuration is done !" << endl;
    }

    void in2out() { // sorting algorithm: reading data from the output "tape" to the buffer,
        in2buff();  // and then transferring the information from the buffer to the output "tape" and back, until all data are in ascending order.
        while (!buff2out()) out2buff();
    }

    int getTotalTime() { // class method returns time spent on magnetic tape emulation
        totalTime = tin.getUsedTime();
        totalTime += tbuf.getUsedTime();
        totalTime += tout.getUsedTime();
        return totalTime;
    }
};

int main()
{
    setlocale(LC_ALL, "rus");
    Sorting sort;

    sort.init(); // requesting the name of the input and output cassette, as well as reading the configuration file

    sort.in2out(); // file sorting itself: reading data from the input "tape" to the buffer "tape", and then sorting by copying and step-by-step sorting between the buffer and output "tapes"

    cout << "\nTotal time : " << sort.getTotalTime() << " ms" << endl; // counts the time spent on copying and sorting data based on the configuration file
}