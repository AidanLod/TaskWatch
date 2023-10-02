# Senior-Thesis
## Purpose	
The purpose of this document is to serve as a guide to designers, developers and testers who are responsible for the engineering of the Automatic Attendance Project. It should give the engineers all of the information necessary to design, develop and test the software.

## Scope
This document contains a complete description of the Automatic Attendance Project. It consists of use cases, functional requirements and nonfunctional requirements. 

## Use Cases

### Name
UC-1: Login
### Summary
The ability to login
### Rationale
Every user has their own unique class schedules, identities, and abilities depending on whether they’re a professor or student. The user needs to be identified for these purposes.
### Users
Student and Professor
### Preconditions
The app is open and the user is a student or faculty member at the college
### Basic Course of Events
User enters correct username
User enters correct password
User presses “Login” button
### Alternative Paths
In step 1 the user enters the wrong username. In this case the user is unable to login and a message pops up telling the user “Wrong username and password combo”.
In step 2 the user enters the wrong password. In this case the user is unable to login and a message pops up telling the user “Wrong username and password combo”.
### Postconditions
User is now logged in
.....................
### Name
UC-2: Check attendance and schedule (Student)
### Summary
Students can check class attendance and schedule.
### Rationale
A student may want to check the attendance and schedule for their classes.
### Users
Student at the college
### Preconditions
The app is open and the user is logged in
### Basic Course of Events
User taps “Classes”
User is brought to a list of their classes
User taps a class
User is shown the days and times the class is held and the number of days they have been absent
### Alternative Paths
In step 2-4 it could turn out the student has no classes. This is not an issue as it will just not show anything listed.
### Postconditions
User now can view their schedule and attendance
...............................................
### Name
UC-3: Check attendance and schedule (Faculty)
### Summary
Faculty can check class attendance and schedule.
### Rationale
A faculty member may want to check the attendance and schedule for their classes.
### Users
Faculty member at the college
### Preconditions
The app is open and the user is logged in
### Basic Course of Events
User taps “Classes”
User is brought to a list of their classes
User taps a class
User is shown the days and times the class is held and the number of days absent for each student in the class
### Alternative Paths
In step 4 it could turn out no student has been absent. In this case it will just show 0 for number of days absent for each student.
### Postconditions
User now can view their schedule and attendance
...............................................
### Name
UC-3: Check attendance and schedule (Faculty)
### Summary
Faculty can check class attendance and schedule.
### Rationale
A faculty member may want to check the attendance and schedule for their classes.
### Users
Faculty member at the college
### Preconditions
The app is open and the user is logged in
### Basic Course of Events
User taps “Classes”
User is brought to a list of their classes
User taps a class
User is shown the days and times the class is held and the number of days absent for each student in the class
### Alternative Paths
In step 4 it could turn out no student has been absent. In this case it will just show 0 for number of days absent for each student.
### Postconditions
User now can view their schedule and attendance
...............................................
### Name
UC-4: Marking present
### Summary
The professors using the app will be able to take attendance without doing anything themselves
### Rationale
A professor does not want to take attendance manually every day because that is extremely tedious
### Users
Faculty member at the college
### Preconditions
The app is running in the background and the user is logged in
### Basic Course of Events
Every student who’s phone is within bluetooth range of the professors phone will be marked present
### Alternative Paths
In Step 1 a student could have forgotten their phone or not be logged in on the app or could not have bluetooth turned on. This is a major issue with no current solution
### Postconditions
Attendance is now taken



