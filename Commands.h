#ifndef SMASH_COMMAND_H_
#define SMASH_COMMAND_H_

#include <vector>
#include <iostream>


#define COMMAND_ARGS_MAX_LENGTH (80)
#define COMMAND_MAX_ARGS (20)

using namespace std;
const std::string WHITESPACE = " \n\r\t\f\v";
const std::string COMPLEX_CHAR = "?*";



/*---------------------CommandParser--------------------------*/


class CommandParser
{
    public:

        enum redirectionType
        {
            NONE,
            APPEND, // >>
            OVERRIDE, // >
            PIPE,  // |
            ERROR_PIPE, // |&
            REDIRECTION_FAIL = -1
        };

        CommandParser() = delete;
        CommandParser(std::string input);
        ~CommandParser() = default;

        std::string getRawCommanad();
        std::string getFirstCommand();
        std::string getSecondCommand();
        std::string getCleanCommand();

        bool getIsBackground();
        bool getIsComplex();
        int getWordCount();
        int getTimeout();
        redirectionType getRedirection();


        std::string& operator[](int index);

    private:      

        const static int TIMEOUT_ARG_COUNT = 2;
        static const int MAX_WORD_COUNT = 21;  // Command + 20 args

        std::string raw_command;
        std::string first_command;
        std::string second_command;
        std::string stripped_flagless_command;
        std::string stripped_words[MAX_WORD_COUNT];
        
        redirectionType redirection;
        bool is_background;
        bool is_complex;
        int word_count;
        int timeout;

        static std::string cleanBackgroundCommand(std::string input);
};


/*---------------------Command--------------------------*/
class Command {

public:

    Command(CommandParser parsed_command);
    int getPid();
    CommandParser getParsedCommand();

    virtual ~Command() = default;
    virtual void execute() = 0;
   

protected:
    int pid;
    CommandParser parsed_command;
    
};



//--------------------------Job----------------------------------//
class Job{
  public:
  enum status {
      RUNNING_FG,
      RUNNING_BG,
      FINISHED
  };

  public:
   Job(int pid);
  ~Job() = default;
  void setJobID(int id);
  int getJobID();
  void setPid(int id);
  int getPid();
  Job::status getCurrentStatus();
  void setCurrentStatus(Job::status status);
  void setCommand(Command *c);
  Command* getCommand();
  void setIsStopped(bool is_stopped);
  bool getIsStopped();


private:
    int jobID;
    int pid;
    bool is_stopped;
    Command* command;
    status currentStatus;


};


//--------------------------JobsList----------------------------//
class JobsList{
  private:
  vector<Job*> list;
  int maxJobID = 0;

  public:
  JobsList() = default;
  ~JobsList();
  void deleteFinishedJobs();
  void addJobToList(Job* j);
  Job *getJobById(int jobId);
  void printJobsList();
};


/*--------------------Built-in commands-----------------------------*/


class ChromptCommand : public Command {
public:
    ChromptCommand(CommandParser parsed_command);
    virtual ~ChromptCommand() = default;
    void execute() override;
};



class ShowPidCommand : public Command {
public:
    ShowPidCommand(CommandParser parsed_command);
    virtual ~ShowPidCommand() = default;
    void execute() override;
};


class PWDCommand : public Command {
public:
    PWDCommand(CommandParser parsed_command);
    virtual ~PWDCommand() = default;
    void execute() override;
};


class CDCommand : public Command {
    std::string& lastPwd;
public:
    CDCommand(CommandParser parsed_command, std::string& prev_PWD);
    virtual ~CDCommand() = default;
    void execute() override;
};


class JobsCommand : public Command {
    JobsList* jobs;
public:
    JobsCommand(CommandParser parsed_command, JobsList* jobs);
    virtual ~JobsCommand() = default;
    void execute() override;
};


class FGCommand : public Command {
    JobsList* jobs;
public:
    FGCommand(CommandParser parsed_command, JobsList* jobs);
    virtual ~FGCommand() = default;
    void execute() override;
};


class QuitCommand : public Command {
    JobsList* jobs;
public:
    QuitCommand(CommandParser parsed_command, JobsList* jobs);
    virtual ~QuitCommand() = default;
    void execute() override;
};


class KillCommand : public Command {
    JobsList* jobs;
public:
    KillCommand(CommandParser parsed_command, JobsList* jobs);
    virtual ~KillCommand() = default;
    void execute() override;
};




/*---------------------------External commands--------------------------------*/

class ExternalCommand : public Command {
    JobsList* jobs;
    //TimeoutList* timeouts;
public:
    //ExternalCommand(CommandParser parsed_command, JobsList* jobs, TimeoutList* timeouts);
    ExternalCommand(CommandParser parsed_command, JobsList* jobs);
    virtual ~ExternalCommand() = default;
    void execute() override;
};


/*-------------------------Special commands-------------------*/

class RedirectionCommand : public Command {
    std::string filePath;
public:
    explicit RedirectionCommand(CommandParser parsed_command);
    virtual ~RedirectionCommand() = default;
    void execute() override;
};

class PipeLineCommand : public Command {
public:
    PipeLineCommand(CommandParser parsed_command);
    virtual ~PipeLineCommand() = default;
    void execute() override;
};


class ChmodCommand : public Command {
    JobsList* jobs;
public:
    ChmodCommand(CommandParser parsed_command, JobsList* jobs);
    virtual ~ChmodCommand() = default;
    void execute() override;
};

class TimeoutCommand : public Command {
    JobsList* jobs;
public:
    TimeoutCommand(CommandParser parsed_command, JobsList* jobs);
    virtual ~TimeoutCommand() = default;
    void execute() override;
};




/*
class JobsList {
 public:
  class JobEntry {
   // TODO: Add your data members
  };
 // TODO: Add your data members
 public:
  JobsList();
  ~JobsList();
  void addJob(Command* cmd, bool isStopped = false);
  void printJobsList();
  void killAllJobs();
  void removeFinishedJobs();
  JobEntry * getJobById(int jobId);
  void removeJobById(int jobId);
  JobEntry * getLastJob(int* lastJobId);
  JobEntry *getLastStoppedJob(int *jobId);
  // TODO: Add extra methods or modify exisitng ones as needed
};
*/


//--------------------------SmallShell------------------------//
class SmallShell {
  private:
  SmallShell();

  public:
  Command *CreateCommand(string cmd_line);
  SmallShell(SmallShell const&)      = delete; // disable copy ctor
  void operator=(SmallShell const&)  = delete; // disable = operator
  static SmallShell& getInstance() // make SmallShell singleton
  {
    static SmallShell instance; // Guaranteed to be destroyed.
    // Instantiated on first use.
    return instance;
  }
  ~SmallShell();
  void executeCommand(string cmd_line);
  // TODO: add extra methods as needed

  //////////////////////
  // our code
  //////////////////////

  private:
  int smash_pid;
  string prompt = "smash";
  string last_dir = "";
  JobsList *jobs_list;
  JobsList *time_out_jobs_list;
  const int MAX_NUM_OF_PROCESSES = 100;
  const int ARGS_MAX = 21;
  const int COMMAND_SIZE_MAX = 80;
  const int PROCESS_NAME_MAX = 50;

  public:
  int get_max_num_of_processes();
  int get_args_max();
  int get_command_size_max();
  int get_process_name_max();
  int get_Smash_Pid();

};




#endif //SMASH_COMMAND_H_
