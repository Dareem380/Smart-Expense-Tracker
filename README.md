Smart Expense Tracker (C)

A menu-driven command-line expense tracker written in pure C. It records your spending, sorts it into categories, draws a text bar chart, and warns you when you are about to exceed your monthly budget. All data is saved to a file, so nothing is lost when you close the program.

Problem Statement
Many students and young professionals don't know where their money goes.
Spreadsheets and apps feel heavy for quick daily tracking.
Without a budget alert, overspending is noticed only at the end of the month.
Our Solution

A lightweight terminal tool that lets you log an expense in seconds, see exactly which category takes most of your money, and get an alert when you approach or cross your budget. It needs no internet, no database, and no external libraries.
Features
Feature	Description
Add expense	Date, category, amount and a short note, with input validation
View all	A clean table of every expense with the running total
Search	Find expenses by keyword in the note or category
Delete	Remove an expense by its ID
Category summary	Percentage breakdown with a text bar chart and your biggest spending area
Monthly report	Total, average, and expense list for any month
Budget alert	Warns at 80% of your monthly budget and alerts when you exceed it
Persistent storage	Data is saved in expenses.txt and budget.txt automatically
Getting Started
Prerequisites
A C compiler such as GCC (MinGW on Windows, or gcc/clang on Linux and macOS)
Compile
bash
gcc expense_tracker.c -o expense_tracker
Run
bash
./expense_tracker          # Linux / macOS
expense_tracker.exe        # Windows
Running in VS Code
Install the C/C++ extension and make sure GCC is installed.
Open expense_tracker.c, then open the terminal with Ctrl+`.
Run the two commands above (compile, then run).
Sample Output
=========================================
       SMART EXPENSE TRACKER (C)
=========================================

  1. Add expense
  2. View all expenses
  3. Search
  4. Delete expense
  5. Category summary
  6. Monthly report
  7. Set monthly budget
  0. Exit
  Choose: 5

--- Spending by Category ---
  Food             1200.00   24.0%  #######
  Transport         800.00   16.0%  #####
  Shopping            0.00    0.0%
  Bills            3000.00   60.0%  ##################
  Entertainment       0.00    0.0%
  Other               0.00    0.0%

  Biggest spending area: Bills (60.0% of total)
  How It Works
Each expense is stored in a struct containing id, date, category, amount and note.
Expenses are kept in an array in memory (up to 1000) and written to a text file after every change.
On start-up, the program reads the file back, so your history is preserved.
The category summary adds up the amounts per category and scales each one to a 30-character bar.
The budget check adds up all expenses for the month of the new entry and compares the total with your budget.
Concepts demonstrated

Structs · arrays · file I/O · string handling · input validation · menu-driven programs · functions and modular design

Project Structure
├── expense_tracker.c    # Complete source code
├── expenses.txt         # Created automatically (your data)
├── budget.txt           # Created automatically (your budget)
└── README.md
Future Scope
Export reports to CSV
Edit an existing expense
Yearly and weekly reports
Password-protected data file
A graphical interface (GTK or a web front end)
Recurring expenses such as rent and subscriptions
Tech Stack
C · Standard library only
