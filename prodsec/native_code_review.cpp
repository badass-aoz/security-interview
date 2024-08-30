#include <iostream>
#include <format>
// assuming the environment is set up correctly
#include <mysql_driver.h>
#include <mysql_connection.h>
#include <cppconn/statement.h>
#include <cppconn/resultset.h>
#include <cppconn/exception.h>

using std::string;
using std::cout;
using std::cin;
using std::endl;
using std::format;

// session.h
class Session {
private:
  string username;
  string password;
  bool is_logged_in;

  string executeSQL(const string &sql);
  string lookupUser(const string &username);

public:
  Session(const string &username, const string &password) : username(username), password(password) {
  }

  ~Session() {
    if (is_logged_in) {
      cout << "Logging out user " << username << endl;
    }
  }

  bool verifyCred(const string &username, const string &password);
  bool makeReservation(const string &startdate, const string &enddate);
  bool viewReservation();
  bool changeReservation(const string &startdate, const string &enddate);
}

// session.cpp
bool Session::verifyCred(const string &username, const string &password) {
  string pwd = lookupUser(username);
  if (pwd == "") {
    // username doesn't exist 
    return false;
  }
  if (strcmp(pwd, password) != 0) {
    // password is incorrect
    return false;
  }
  return true;
}

bool Session::makeReservation(const string &startdate, const string &enddate) {
  string sql = format("INSERT INTO reservations VALUES (\"%s\", \"%s\", \"%s\")", startdate, enddate, username);
  try {
    executeSQL(sql);
    cout << "Reservation made successfully" << endl;
  } catch (sql::SQLExeption &e) {
    return false;
  }
  return true;
}

bool Session::viewReservation() {
  string sql = format("SELECT startdate, enddate FROM reservations WHERE username = \"%s\"", username);
  try {
    std::unique_ptr<sql::ResultSet> res = executeSQL(sql);
    cout << "Reservations for user " << username << ":" << endl;
    cout << "Start Date\tEnd Date" << endl;
    cout << res->getString("startdate") << "\t" << res->getString("enddate") << endl;
  } catch (sql::SQLException &e) {
    return false;
  }
  return true;
}

bool Session::changeReservation(const string &startdate, const string &enddate) {
  string sql = format("UPDATE reservations SET startdate = \"%s\" AND enddate = \"%s\" WHERE username = %s", startdate, enddate, username);
  try {
    executeSQL(sql);
    cout << "Reservation updated successfully" << endl;
  } catch (sql::SQLExeption &e) {
    return false;
  }
  return true;
}

string Session::lookupUser(const string &username) {
  string password;
  string sql = format("SELECT password FROM users WHERE username = \"%s\"", username);
  try {
    password = executeSQL(sql)->getString(1);
  } catch (sql::SQLException &e) {
    std::cerr << "SQLException: " << e.what();
    std::cerr << " (MySQL error code: " << e.getErrorCode();
    std::cerr << ", SQLState: " << e.getSQLState() << " )" << std::endl;
  }
  return password;
}

std::unique_ptr<sql::ResultSet> Session::executeSQL(const string &sql) {
  sql::mysql::MySQL_Driver *driver = sql::mysql::get_mysql_driver_instance();

  std::unique_ptr<sql::Connection> con(driver->connect("tcp://127.0.0.1:3306", "admin", "R00tP@ssw0rd"));

  // Connect to the specific database
  con->setSchema("ReservationUsers");

// Create a statement object
  std::unique_ptr<sql::Statement> stmt(con->createStatement());
  std::unique_ptr<sql::ResultSet> res(stmt->executeQuery(sql));
  return res;
}


// main.cpp
int main(unsigned int argc, char** argv) {
  if (argc != 3) {
    cout << "Usage: <binary> username password" << endl;
    return -1;
  }
  char *username = argv[1];
  char *password = argv[2];
  Session *session = new Session(username, password);

  if (!session->verifyCred()) {
    cout << "username incorrect" << endl;
    return -1;
  }
  
  char *welcomeMessage = new char[50];
  // check for buffer overflow
  if (strlen(username) + 14 > 50) {
    cout << "username is too long" << endl;
    return -1;
  }
  sprintf(welcomeMessage, "Welcome user %s!", username);
  cout << welcomeMessage << endl;
  int choice;
  while (true) {
    cout << "What would you like to do?" << endl;
    cout << "[1] Make a reservation" << endl;
    cout << "[2] View reservation" << endl;
    cout << "[3] Change an existing reservation" << endl;
    std >> choice;
    switch (choice) {
    case 1:
      session->makeReservation();
      break;
    case 2:
      session->viewReservation();
      break;
    case 3:
      session->changeReservation();
    default:
      cout << "invalid choice. Use Ctrl+C to exit" << endl;
      free(session);
    }
  }
  free(session);
  return 0;
}
