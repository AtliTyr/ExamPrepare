#include <iostream>
#include <string>
#include <fstream>
#include <filesystem>
#include <iomanip>

using namespace std;

#define END_OF_RECORD '\0'

static unsigned int FNVHash(string str) {
	const unsigned int fnv_prime = 0x811C9DC5;
	unsigned int hash = 0;
	unsigned int i = 0;
	unsigned int len = str.length();

	for (i = 0; i < len; i++)
	{
		hash *= fnv_prime;
		hash ^= (str[i]);
	}

	return hash;
}

class File
{
private:
	fstream f;
	std::string pathToFile;
	short stringAmount = 0;
public:
	File() = default;
	File(std::string fName)
	{
		//pathToFile = fName;
		//cout << "pathToFile: " << fName << endl;
		open(fName);
	}
	void open(std::string fName)
	{
		pathToFile = fName;
		f.open(fName, ios::in | ios::out | ios::app);
		if (isOpened())
		{
			cout << "OPENED SUCCESSFULLY\n";
			int tmp = 0;
			f.clear();
			f.seekg(0, ios::beg);
			while (!isEOF())
			{
				std::string c;
				std::getline(f, c, f.widen(END_OF_RECORD));
				tmp++;
			}
			stringAmount = tmp - 1;
		}
		else
			cout << "ERROR OPENING\n";
	}
	~File()
	{
		cout << "FILE DESTRUCTOR\n";
		if(isOpened())
			f.close();	
	}
	short getStringAmount() const
	{
		return stringAmount;
	}
	bool isEOF() const
	{
		return f.eof();
	}
	bool isOpened() const
	{
		return f.is_open();
	}
	string getPathToFile() const
	{
		return pathToFile;
	}
	void addNewRecord(string Rec)
	{
		f.clear();
		f.seekp(0, f.end);
		f << Rec << END_OF_RECORD;
		stringAmount++;
	}
	std::string getRecordByInd(int ind)
	{
		if (ind < 0 || ind >= stringAmount)
			return string();
		int inc = 0;
		setFileForReading();
		/*f.clear();
		f.seekg(0, f.beg);*/
		cout << "Called\n";
		while (!isEOF())
		{
			string tmp;
			std::getline(f, tmp, f.widen(END_OF_RECORD));
			if (inc == ind)
				return tmp;
			inc++;
		}
		return string();
	}
	void copyFile(string pathToCopyfromFile)
	{
		fstream other(pathToCopyfromFile, ios::in);
		other.clear();
		other.seekg(0, ios::beg);
		
		f.close();
		f.open(pathToFile, ios::in | ios::out | ios::trunc);
		f.clear();
		f.seekp(0, ios::end);

		while (!other.eof())
		{
			string tmp;
			getline(other, tmp, other.widen(END_OF_RECORD));
			if (char(other.peek()) == ' ')
				other.get();
			f << tmp << END_OF_RECORD;
		}
		other.close();
	}
	void deleteRecord(int ind)
	{
		if (ind < 0 || ind >= stringAmount)
			return;

		fstream tmp("tmp.txt", ios::app);
		int inc = 0;
		/*f.clear();
		f.seekg(f.beg);*/
		setFileForReading();
		while (!isEOF())
		{
			string t;
			std::getline(f, t, f.widen(END_OF_RECORD));
			if (char(f.peek()) == ' ')
				f.get();
			if (inc != ind)
				tmp << t << END_OF_RECORD;
			if (isEOF())
				break;
			inc++;
		}
		stringAmount--;
		tmp.close();
		copyFile("tmp.txt");
		remove("tmp.txt");	
	}
	void display()
	{
		setFileForReading();
		while (!isEOF())
		{
			string tmp;
			getline(f, tmp, END_OF_RECORD);
			cout << tmp << endl;
		}
	}
	void setFileForReading()
	{
		f.clear();
		f.seekg(0, ios::beg);
	}
	std::string getSingleStringWithNoPreparation()
	{
		string s;
		std::getline(f, s, END_OF_RECORD);
		if (char(f.peek()) == ' ')
			f.get();

		return s;
	}
	void editString(int ind, string newString)
	{
		if (ind < 0 || ind >= stringAmount)
			return;

		fstream tmp("tmp.txt", ios::app);
		int inc = 0;
		/*f.clear();
		f.seekg(f.beg);*/
		setFileForReading();
		while (!isEOF())
		{
			string t;
			std::getline(f, t, f.widen(END_OF_RECORD));
			if (char(f.peek()) == ' ')
				f.get();
			if (inc != ind)
				tmp << t << END_OF_RECORD;
			else
				tmp << newString << END_OF_RECORD;
			if (isEOF())
				break;
			inc++;
		}
		stringAmount--;
		tmp.close();
		copyFile("tmp.txt");
		remove("tmp.txt");
	}
};
class SectionOfDatabase
{
private:
	unsigned int Section_ID;
	File QuestionsF;
	File AnswersF;
public:
	SectionOfDatabase() = default;
	SectionOfDatabase(unsigned int H_id)
	{
		Section_ID = H_id;

		QuestionsF.open(to_string(H_id).append("Q\0"));
		AnswersF.open(to_string(H_id).append("A\0"));
	}
	~SectionOfDatabase() = default; 
	unsigned int getSection_ID() const
	{
		return Section_ID;
	}
	void AddNewQuestion(string Q)
	{
		QuestionsF.addNewRecord(Q);
		AnswersF.addNewRecord("No Answer Found");
	}
	//void AddAnswer(string A)
	//{
	//	AnswersF.addNewRecord(A);
	//}
	void DeleteQuestion(int ind)
	{
		QuestionsF.deleteRecord(ind);
		AnswersF.deleteRecord(ind);
	}
	void DisplayQuestionsF()
	{
		QuestionsF.setFileForReading();

		int counter = 0;
		cout << "                     Question File                        " << endl;
		cout << "__________________________________________________________" << endl;
		while (!QuestionsF.isEOF())
		{	
			const int off = 50;
			string tmp = QuestionsF.getSingleStringWithNoPreparation();

			int l = tmp.length();
			int inc = 0;
			if (l > off)
			{
				cout << "|" << setw(2) << counter++ << " | " << setw(off) << tmp.substr(inc, off) << " |" << endl;
				l -= off;
				inc += off;
				while (l > off)
				{
					cout << "|   | " << setw(off) << tmp.substr(inc, off) << " |" << endl;
					l -= off;
					inc += off;
				}
				cout << "|   | " << setw(off) << tmp.substr(inc, l) << " |" << endl;
			}
			else
				cout << "|" << setw(2) << counter++ << " | " << setw(off) << tmp << " |" << endl;
			cout << "|___|____________________________________________________|" << endl;
		}
	}
	void DisplayAnswersF()
	{
		AnswersF.setFileForReading();

		int counter = 0;
		cout << "                     Answer File                          " << endl;
		cout << "__________________________________________________________" << endl;
		while (!AnswersF.isEOF())
		{
			const int off = 50;
			string tmp = AnswersF.getSingleStringWithNoPreparation();

			int l = tmp.length();
			int inc = 0;
			if (l > off)
			{
				cout << "|" << setw(2) << counter++ << " | " << setw(off) << tmp.substr(inc, off) << " |" << endl;
				l -= off;
				inc += off;
				while (l > off)
				{
					cout << "|   | " << setw(off) << tmp.substr(inc, off) << " |" << endl;
					l -= off;
					inc += off;
				}
				cout << "|   | " << setw(off) << tmp.substr(inc, l) << " |" << endl;
			}
			else
				cout << "|" << setw(2) << counter++ << " | " << setw(off) << tmp << " |" << endl;
			cout << "|___|____________________________________________________|" << endl;
		}
	}
	void EditFileQ(int ind, string newStr)
	{
		QuestionsF.editString(ind, newStr);
	}
	void EditFileA(int ind, string newStr)
	{
		AnswersF.editString(ind, newStr);
	}
};

class DatabaseControl
{
private:
	vector<SectionOfDatabase*> groupList;
	vector<string> groupNames;
public:
	DatabaseControl() = default;
	~DatabaseControl()
	{
		if (groupList.size() != 0)
			for_each(groupList.begin(), groupList.end(), [](SectionOfDatabase* ptr) {delete ptr; });
	}
	bool checkIfGroupExists(string groupName) const
	{
		if (any_of(groupNames.begin(), groupNames.end(), [groupName](string i) {return groupName == i; }))
			return true;
		return false;
	}
	bool createNewGroup(string groupName)
	{
		if (checkIfGroupExists(groupName) == true)
		{
			cout << "Group \"" << groupName << "\" already exists!" << endl;
			return false;
		}
		groupNames.push_back(groupName);
		groupList.push_back(new SectionOfDatabase(FNVHash(groupName)));
		return true;
	}
	void displayGroupList() const
	{
		for_each(groupNames.begin(), groupNames.end(), [](string tmp) { cout << tmp << endl; });
	}
	void deleteGroup(string groupName)
	{
		if (checkIfGroupExists(groupName) != true)
		{
			cout << "Group \"" << groupName << "\" doesn't exists!" << endl;
			return;
		}
		for (vector<string>::iterator it = groupNames.begin(); it != groupNames.end(); it++)
			if (*it == groupName)
			{
				groupNames.erase(it);
				break;
			}
		unsigned int groupHash = FNVHash(groupName);
		for(auto it = groupList.begin(); it != groupList.end(); it++)
			if ((*it)->getSection_ID() == groupHash)
			{
				delete (*it);
				groupList.erase(it);
				break;
			}
		remove(string(groupName + "Q").c_str());
		remove(string(groupName + "A").c_str());
	}
};
//
//class IUInterface
//{
//private:
//
//public:
//
//};

int main()
{
	setlocale(LC_ALL, "rus");
	//SectionOfDatabase s_db(FNVHash("group A"));
	////s_db.EditFileQ(2, "Предел монотонной ограниченной последовательности, функции. ");
	/*s_db.DisplayQuestionsF();*/
	DatabaseControl db;
	//db.createNewGroup("group A");
	db.createNewGroup("group A");
	//db.displayGroupList();
	//db.deleteGroup("group A");
	db.displayGroupList();
	return 0;
}