#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_RECORDS 100
#define PASSWORD "sai@123"

struct clientData
{
    unsigned int acctNum;
    char lastName[20];
    char firstName[20];
    double balance;
};

/* Function Prototypes */
int login();

void initializeFile(FILE *fPtr);

void createAccount(FILE *fPtr);
void depositMoney(FILE *fPtr);
void withdrawMoney(FILE *fPtr);
void transferMoney(FILE *fPtr);

void deleteAccount(FILE *fPtr);
void searchAccount(FILE *fPtr);
void displayAllAccounts(FILE *fPtr);
void totalBankBalance(FILE *fPtr);

unsigned int menu();

/* MAIN FUNCTION */
int main()
{
    FILE *cfPtr;
    unsigned int choice;

    /* Password Protection */
    if (!login())
    {
        printf("Too many incorrect attempts.\n");
        return 0;
    }

    /* Open File */
    if ((cfPtr = fopen("bank.dat", "rb+")) == NULL)
    {
        cfPtr = fopen("bank.dat", "wb+");

        if (cfPtr == NULL)
        {
            printf("File could not be opened.\n");
            return 1;
        }

        initializeFile(cfPtr);
    }

    while ((choice = menu()) != 9)
    {
        switch (choice)
        {
        case 1:
            createAccount(cfPtr);
            break;

        case 2:
            depositMoney(cfPtr);
            break;

        case 3:
            withdrawMoney(cfPtr);
            break;

        case 4:
            transferMoney(cfPtr);
            break;

        case 5:
            deleteAccount(cfPtr);
            break;

        case 6:
            searchAccount(cfPtr);
            break;

        case 7:
            displayAllAccounts(cfPtr);
            break;

        case 8:
            totalBankBalance(cfPtr);
            break;

        default:
            printf("Invalid Choice.\n");
        }
    }

    fclose(cfPtr);

    printf("\nThank You...\n");

    return 0;
}

/* PASSWORD LOGIN */
int login()
{
    char pass[20];
    int attempts = 3;

    while (attempts > 0)
    {
        printf("Enter Password: ");
        scanf("%19s", pass);

        if (strcmp(pass, PASSWORD) == 0)
        {
            printf("\nLogin Successful.\n");
            return 1;
        }

        attempts--;

        printf("Incorrect Password.\n");
        printf("Remaining Attempts: %d\n", attempts);
    }

    return 0;
}

/* INITIALIZE FILE */
void initializeFile(FILE *fPtr)
{
    struct clientData blank = {0, "", "", 0.0};

    rewind(fPtr);

    for (int i = 0; i < MAX_RECORDS; i++)
    {
        fwrite(&blank, sizeof(struct clientData), 1, fPtr);
    }

    fflush(fPtr);
}

/* CREATE ACCOUNT */
void createAccount(FILE *fPtr)
{
    struct clientData client = {0, "", "", 0.0};
    unsigned int acc;

    printf("\nEnter Account Number (1-100): ");
    scanf("%u", &acc);

    if (acc < 1 || acc > MAX_RECORDS)
    {
        printf("Invalid Account Number.\n");
        return;
    }

    fseek(fPtr, (acc - 1) * sizeof(struct clientData), SEEK_SET);
    fread(&client, sizeof(struct clientData), 1, fPtr);

    if (client.acctNum != 0)
    {
        printf("Account Already Exists.\n");
        return;
    }

    client.acctNum = acc;

    printf("Enter Last Name: ");
    scanf("%19s", client.lastName);

    printf("Enter First Name: ");
    scanf("%19s", client.firstName);

    printf("Enter Initial Balance: ");
    scanf("%lf", &client.balance);

    if (client.balance < 0)
    {
        printf("Balance Cannot Be Negative.\n");
        return;
    }

    fseek(fPtr, (acc - 1) * sizeof(struct clientData), SEEK_SET);
    fwrite(&client, sizeof(struct clientData), 1, fPtr);

    fflush(fPtr);

    printf("Account Created Successfully.\n");
}

/* DEPOSIT MONEY */
void depositMoney(FILE *fPtr)
{
    struct clientData client;
    unsigned int acc;
    double amount;

    printf("\nEnter Account Number: ");
    scanf("%u", &acc);

    fseek(fPtr, (acc - 1) * sizeof(struct clientData), SEEK_SET);
    fread(&client, sizeof(struct clientData), 1, fPtr);

    if (client.acctNum == 0)
    {
        printf("Account Not Found.\n");
        return;
    }

    printf("Current Balance: %.2lf\n", client.balance);

    printf("Enter Deposit Amount: ");
    scanf("%lf", &amount);

    if (amount <= 0)
    {
        printf("Invalid Amount.\n");
        return;
    }

    client.balance += amount;

    fseek(fPtr, (acc - 1) * sizeof(struct clientData), SEEK_SET);
    fwrite(&client, sizeof(struct clientData), 1, fPtr);

    fflush(fPtr);

    printf("Amount Deposited Successfully.\n");
    printf("Updated Balance: %.2lf\n", client.balance);
}

/* WITHDRAW MONEY */
void withdrawMoney(FILE *fPtr)
{
    struct clientData client;
    unsigned int acc;
    double amount;

    printf("\nEnter Account Number: ");
    scanf("%u", &acc);

    fseek(fPtr, (acc - 1) * sizeof(struct clientData), SEEK_SET);
    fread(&client, sizeof(struct clientData), 1, fPtr);

    if (client.acctNum == 0)
    {
        printf("Account Not Found.\n");
        return;
    }

    printf("Current Balance: %.2lf\n", client.balance);

    printf("Enter Withdraw Amount: ");
    scanf("%lf", &amount);

    if (amount <= 0)
    {
        printf("Invalid Amount.\n");
        return;
    }

    if (amount > client.balance)
    {
        printf("Insufficient Balance.\n");
        return;
    }

    client.balance -= amount;

    fseek(fPtr, (acc - 1) * sizeof(struct clientData), SEEK_SET);
    fwrite(&client, sizeof(struct clientData), 1, fPtr);

    fflush(fPtr);

    printf("Withdrawal Successful.\n");
    printf("Remaining Balance: %.2lf\n", client.balance);
}

/* TRANSFER MONEY */
void transferMoney(FILE *fPtr)
{
    struct clientData sender, receiver;

    unsigned int fromAcc, toAcc;
    double amount;

    printf("\nEnter Sender Account Number: ");
    scanf("%u", &fromAcc);

    printf("Enter Receiver Account Number: ");
    scanf("%u", &toAcc);

    printf("Enter Amount To Transfer: ");
    scanf("%lf", &amount);

    if (amount <= 0)
    {
        printf("Invalid Amount.\n");
        return;
    }

    /* Read Sender */
    fseek(fPtr, (fromAcc - 1) * sizeof(struct clientData), SEEK_SET);
    fread(&sender, sizeof(struct clientData), 1, fPtr);

    /* Read Receiver */
    fseek(fPtr, (toAcc - 1) * sizeof(struct clientData), SEEK_SET);
    fread(&receiver, sizeof(struct clientData), 1, fPtr);

    if (sender.acctNum == 0 || receiver.acctNum == 0)
    {
        printf("Invalid Account Number.\n");
        return;
    }

    if (sender.balance < amount)
    {
        printf("Insufficient Balance.\n");
        return;
    }

    sender.balance -= amount;
    receiver.balance += amount;

    /* Update Sender */
    fseek(fPtr, (fromAcc - 1) * sizeof(struct clientData), SEEK_SET);
    fwrite(&sender, sizeof(struct clientData), 1, fPtr);

    /* Update Receiver */
    fseek(fPtr, (toAcc - 1) * sizeof(struct clientData), SEEK_SET);
    fwrite(&receiver, sizeof(struct clientData), 1, fPtr);

    fflush(fPtr);

    printf("Money Transferred Successfully.\n");
}

/* DELETE ACCOUNT */
void deleteAccount(FILE *fPtr)
{
    struct clientData blank = {0, "", "", 0.0};
    struct clientData client;

    unsigned int acc;

    printf("\nEnter Account Number To Delete: ");
    scanf("%u", &acc);

    fseek(fPtr, (acc - 1) * sizeof(struct clientData), SEEK_SET);
    fread(&client, sizeof(struct clientData), 1, fPtr);

    if (client.acctNum == 0)
    {
        printf("Account Not Found.\n");
        return;
    }

    fseek(fPtr, (acc - 1) * sizeof(struct clientData), SEEK_SET);
    fwrite(&blank, sizeof(struct clientData), 1, fPtr);

    fflush(fPtr);

    printf("Account Deleted Successfully.\n");
}

/* SEARCH ACCOUNT */
void searchAccount(FILE *fPtr)
{
    struct clientData client;
    unsigned int acc;

    printf("\nEnter Account Number To Search: ");
    scanf("%u", &acc);

    fseek(fPtr, (acc - 1) * sizeof(struct clientData), SEEK_SET);
    fread(&client, sizeof(struct clientData), 1, fPtr);

    if (client.acctNum == 0)
    {
        printf("Account Not Found.\n");
    }
    else
    {
        printf("\n===== ACCOUNT DETAILS =====\n");

        printf("Account Number : %u\n", client.acctNum);
        printf("Last Name      : %s\n", client.lastName);
        printf("First Name     : %s\n", client.firstName);
        printf("Balance        : %.2lf\n", client.balance);
    }
}

/* DISPLAY ALL ACCOUNTS */
void displayAllAccounts(FILE *fPtr)
{
    struct clientData client;

    rewind(fPtr);

    printf("\n====================================================\n");
    printf("%-10s %-15s %-15s %-10s\n",
           "Account", "Last Name", "First Name", "Balance");
    printf("====================================================\n");

    while (fread(&client, sizeof(struct clientData), 1, fPtr))
    {
        if (client.acctNum != 0)
        {
            printf("%-10u %-15s %-15s %-10.2lf\n",
                   client.acctNum,
                   client.lastName,
                   client.firstName,
                   client.balance);
        }
    }
}

/* TOTAL BANK BALANCE */
void totalBankBalance(FILE *fPtr)
{
    struct clientData client;
    double total = 0;

    rewind(fPtr);

    while (fread(&client, sizeof(struct clientData), 1, fPtr))
    {
        if (client.acctNum != 0)
        {
            total += client.balance;
        }
    }

    printf("\nTotal Bank Balance = %.2lf\n", total);
}

/* MENU */
unsigned int menu()
{
    unsigned int choice;

    printf("\n========== BANK MANAGEMENT SYSTEM ==========\n");

    printf("1. Create Account\n");
    printf("2. Deposit Money\n");
    printf("3. Withdraw Money\n");
    printf("4. Transfer Money\n");
    printf("5. Delete Account\n");
    printf("6. Search Account\n");
    printf("7. Display All Accounts\n");
    printf("8. Display Total Bank Balance\n");
    printf("9. Exit\n");

    printf("Enter Your Choice: ");
    scanf("%u", &choice);

    return choice;
}