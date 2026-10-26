/*
 * Smart Expense Tracker  -  written in C
 * ---------------------------------------
 * Features : add / view / search / delete expenses, category summary with
 *            text bar chart, monthly report, budget alert, data saved in a file.
 *
 * Compile  : gcc expense_tracker.c -o expense_tracker
 * Run      : ./expense_tracker        (Windows: expense_tracker.exe)
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_EXPENSES 1000
#define DATA_FILE    "expenses.txt"
#define BUDGET_FILE  "budget.txt"
#define NUM_CATS     6

typedef struct {
    int    id;
    int    day, month, year;
    int    category;          /* index into CATEGORIES */
    float  amount;
    char   note[50];
} Expense;

static const char *CATEGORIES[NUM_CATS] = {
    "Food", "Transport", "Shopping", "Bills", "Entertainment", "Other"
};

static Expense expenses[MAX_EXPENSES];
static int     count     = 0;
static int     next_id   = 1;
static float   budget    = 0.0f;     /* monthly budget, 0 = not set */

/* ---------------------------------------------------------------- input helpers */
static void read_line(char *buf, int size) {
    if (fgets(buf, size, stdin) == NULL) { buf[0] = '\0'; return; }
    buf[strcspn(buf, "\n")] = '\0';
}

static int read_int(const char *prompt, int lo, int hi) {
    char line[64];
    int  v;
    for (;;) {
        printf("%s", prompt);
        if (fgets(line, sizeof line, stdin) == NULL) exit(0);   /* end of input */
        if (sscanf(line, "%d", &v) == 1 && v >= lo && v <= hi) return v;
        printf("  Please enter a number between %d and %d.\n", lo, hi);
    }
}

static float read_float(const char *prompt) {
    char  line[64];
    float v;
    for (;;) {
        printf("%s", prompt);
        if (fgets(line, sizeof line, stdin) == NULL) exit(0);
        if (sscanf(line, "%f", &v) == 1 && v > 0) return v;
        printf("  Please enter a positive amount.\n");
    }
}

/* ---------------------------------------------------------------- file storage */
static void save_data(void) {
    FILE *f = fopen(DATA_FILE, "w");
    if (!f) { printf("  Could not save data!\n"); return; }
    for (int i = 0; i < count; i++) {
        Expense *e = &expenses[i];
        fprintf(f, "%d|%d|%d|%d|%d|%.2f|%s\n",
                e->id, e->day, e->month, e->year, e->category, e->amount, e->note);
    }
    fclose(f);

    f = fopen(BUDGET_FILE, "w");
    if (f) { fprintf(f, "%.2f\n", budget); fclose(f); }
}

static void load_data(void) {
    char line[200];
    FILE *f = fopen(DATA_FILE, "r");
    if (f) {
        while (count < MAX_EXPENSES && fgets(line, sizeof line, f)) {
            Expense e;
            memset(&e, 0, sizeof e);
            if (sscanf(line, "%d|%d|%d|%d|%d|%f|%49[^\n]",
                       &e.id, &e.day, &e.month, &e.year,
                       &e.category, &e.amount, e.note) >= 6) {
                if (e.category < 0 || e.category >= NUM_CATS) e.category = NUM_CATS - 1;
                expenses[count++] = e;
                if (e.id >= next_id) next_id = e.id + 1;
            }
        }
        fclose(f);
    }
    f = fopen(BUDGET_FILE, "r");
    if (f) { if (fscanf(f, "%f", &budget) != 1) budget = 0; fclose(f); }
}

/* ---------------------------------------------------------------- features */
static void print_header(void) {
    printf("\n  %-4s %-11s %-14s %10s  %s\n", "ID", "Date", "Category", "Amount", "Note");
    printf("  ---- ----------- -------------- ----------  ------------------\n");
}

static void print_expense(const Expense *e) {
    printf("  %-4d %02d-%02d-%04d  %-14s %10.2f  %s\n",
           e->id, e->day, e->month, e->year, CATEGORIES[e->category], e->amount, e->note);
}

static void add_expense(void) {
    if (count >= MAX_EXPENSES) { printf("  Storage full!\n"); return; }
    Expense e;
    memset(&e, 0, sizeof e);

    printf("\n--- Add Expense ---\n");
    e.day   = read_int("  Day   (1-31)   : ", 1, 31);
    e.month = read_int("  Month (1-12)   : ", 1, 12);
    e.year  = read_int("  Year  (e.g. 2025): ", 2000, 2100);

    for (int i = 0; i < NUM_CATS; i++) printf("   %d. %s\n", i + 1, CATEGORIES[i]);
    e.category = read_int("  Category (1-6) : ", 1, NUM_CATS) - 1;
    e.amount   = read_float("  Amount         : ");
    printf("  Note           : ");
    read_line(e.note, sizeof e.note);
    if (e.note[0] == '\0') strcpy(e.note, "-");
    for (char *p = e.note; *p; p++) if (*p == '|') *p = '/';   /* keep file format safe */

    e.id = next_id++;
    expenses[count++] = e;
    save_data();
    printf("  Expense #%d saved.\n", e.id);

    /* budget alert for that month */
    if (budget > 0) {
        float total = 0;
        for (int i = 0; i < count; i++)
            if (expenses[i].month == e.month && expenses[i].year == e.year)
                total += expenses[i].amount;
        if (total > budget)
            printf("  [!] ALERT: You have exceeded your budget of %.2f (spent %.2f)\n", budget, total);
        else if (total > 0.8f * budget)
            printf("  [!] Warning: You have used %.0f%% of your monthly budget.\n", 100 * total / budget);
    }
}

static void view_all(void) {
    printf("\n--- All Expenses ---\n");
    if (count == 0) { printf("  No expenses recorded yet.\n"); return; }
    print_header();
    float total = 0;
    for (int i = 0; i < count; i++) { print_expense(&expenses[i]); total += expenses[i].amount; }
    printf("  ---- ----------- -------------- ----------\n");
    printf("  %-31s %10.2f\n", "TOTAL", total);
}

static void search_expenses(void) {
    char key[50];
    printf("\n--- Search (note or category) ---\n  Keyword: ");
    read_line(key, sizeof key);
    for (char *p = key; *p; p++) *p = (char)tolower((unsigned char)*p);
    if (key[0] == '\0') return;

    int found = 0;
    for (int i = 0; i < count; i++) {
        char note[50], cat[20];
        strcpy(note, expenses[i].note);
        strcpy(cat, CATEGORIES[expenses[i].category]);
        for (char *p = note; *p; p++) *p = (char)tolower((unsigned char)*p);
        for (char *p = cat;  *p; p++) *p = (char)tolower((unsigned char)*p);
        if (strstr(note, key) || strstr(cat, key)) {
            if (!found) print_header();
            print_expense(&expenses[i]);
            found++;
        }
    }
    if (!found) printf("  No matching expenses.\n");
}

static void delete_expense(void) {
    printf("\n--- Delete Expense ---\n");
    int id = read_int("  Enter expense ID: ", 1, 1000000);
    for (int i = 0; i < count; i++) {
        if (expenses[i].id == id) {
            for (int j = i; j < count - 1; j++) expenses[j] = expenses[j + 1];
            count--;
            save_data();
            printf("  Expense #%d deleted.\n", id);
            return;
        }
    }
    printf("  ID not found.\n");
}

static void category_summary(void) {
    float totals[NUM_CATS] = {0}, grand = 0;
    for (int i = 0; i < count; i++) {
        totals[expenses[i].category] += expenses[i].amount;
        grand += expenses[i].amount;
    }
    printf("\n--- Spending by Category ---\n");
    if (grand == 0) { printf("  No data yet.\n"); return; }

    int top = 0;
    for (int c = 0; c < NUM_CATS; c++) {
        int bars = (int)(totals[c] / grand * 30 + 0.5f);
        printf("  %-14s %9.2f  %5.1f%%  ", CATEGORIES[c], totals[c], 100 * totals[c] / grand);
        for (int b = 0; b < bars; b++) putchar('#');
        putchar('\n');
        if (totals[c] > totals[top]) top = c;
    }
    printf("\n  Biggest spending area: %s (%.1f%% of total)\n", CATEGORIES[top], 100 * totals[top] / grand);
}

static void monthly_report(void) {
    printf("\n--- Monthly Report ---\n");
    int m = read_int("  Month (1-12)    : ", 1, 12);
    int y = read_int("  Year (e.g. 2025): ", 2000, 2100);

    float total = 0, by_cat[NUM_CATS] = {0};
    int n = 0;
    for (int i = 0; i < count; i++) {
        if (expenses[i].month == m && expenses[i].year == y) {
            if (n == 0) print_header();
            print_expense(&expenses[i]);
            total += expenses[i].amount;
            by_cat[expenses[i].category] += expenses[i].amount;
            n++;
        }
    }
    if (n == 0) { printf("  No expenses in %02d/%d.\n", m, y); return; }

    printf("\n  Total for %02d/%d : %.2f  (%d expenses, avg %.2f)\n", m, y, total, n, total / n);
    if (budget > 0) {
        printf("  Budget            : %.2f  -> %s by %.2f\n", budget,
               total <= budget ? "UNDER" : "OVER", total <= budget ? budget - total : total - budget);
    }
    (void)by_cat;
}

static void set_budget(void) {
    printf("\n--- Set Monthly Budget ---\n");
    budget = read_float("  Budget amount: ");
    save_data();
    printf("  Monthly budget set to %.2f\n", budget);
}

/* ---------------------------------------------------------------- main */
int main(void) {
    load_data();
    printf("=========================================\n");
    printf("       SMART EXPENSE TRACKER (C)\n");
    printf("=========================================\n");
    printf("  Loaded %d saved expense(s).\n", count);

    for (;;) {
        printf("\n  1. Add expense\n  2. View all expenses\n  3. Search\n"
               "  4. Delete expense\n  5. Category summary\n  6. Monthly report\n"
               "  7. Set monthly budget\n  0. Exit\n");
        int choice = read_int("  Choose: ", 0, 7);
        switch (choice) {
            case 1: add_expense();      break;
            case 2: view_all();         break;
            case 3: search_expenses();  break;
            case 4: delete_expense();   break;
            case 5: category_summary(); break;
            case 6: monthly_report();   break;
            case 7: set_budget();       break;
            case 0: printf("  Goodbye! Your data is saved in %s\n", DATA_FILE); return 0;
        }
    }
}
