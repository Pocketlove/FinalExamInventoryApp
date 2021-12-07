/*---------------------------------------\

		Work of Timothy Zheludkov
		   Student #110016720

		8th April, 2020  -  9:05AM

\---------------------------------------*/


/*-----------------------------------------------------------------------------------------------------------\

Declaration of Personal Academic Integrity – COMP 1410 – Winter 2020

As a consequence of the current COVID-19 health crisis and disruption of normal academic procedures,
the University Senate, and President, regular Final Examinations have been cancelled and course
instructors have been tasked with developing an alternative approach to the final evaluation requirement.
For the course COMP 1410, this requirement involves answering a set of questions and submitting
answers that are arrived at independently by each student. Thus, the final requirement is essentially an
open-book, take-home Final Examination.

Academic integrity is fundamental to learning and scholarship at the University of Windsor. Academic
honesty and responsibility are fundamental to good scholarship and learning. As members of this
academic community, you have a responsibility to conduct yourself in accordance with these
expectations. The final requirement for individual evaluation in this course is designed to give you an
opportunity to demonstrate what you have learned about important skills and concepts presented in this
course as part of the stated syllabus.

Completing and submitting the exam answers independently, without any help from other persons or
sources, demands the highest standards of academic integrity and honesty on the part of each, and every,
student. It is the responsibility of each student to read and be aware of the meaning of academic integrity
at the University of Windsor. There are two documents, approved by the University Senate, that apply
and describe the kinds of conduct and sanctions; these can be found at the link:
http://www.uwindsor.ca/academic-integrity/306/student-resources. There are two parts – the Student
Code of Conduct and the Bylaw 31: Academic Integrity.
This examination necessarily allows for use of books and online resources. However, it is assumed that
students will work independently - it is strictly forbidden to collaborate or copy from other students or any
other person. Any such collaboration or copying is considered a violation of the code of conduct and
unethical behaviour.

When you have completed your Final Examination answers, and are ready to submit your work, you must
complete and sign this declaration in the spaces provided below, attesting to your own honesty in
completing the examination on your own and that you understand the meaning of academic integrity in
the context of this Final Examination. If you are submitting a complete C language program file, you may
copy and paste this document into your file and complete the signature portion using your student ID
(which should be private and unique).

PRINT FULL NAME: Timothy Zheludkov
SIGNATURE: 110016720


If you have any questions about what is, or what is not, permitted in this course, please do not hesitate to
contact your instructor.
\-----------------------------------------------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//Macros defining widely used constants for ease of maintenance

#define MAXRECEIPTSIZE 100
#define MAXSTRINGSIZE 256
#define MAXFILESIZE 2048
#define MAXCODELENGTH 4
#define CATEGORYCOUNT 8
#define MAXROOTLENGTH 32

#define CATEGORYFILEPATH "D:\\FinalExam\\CategoryName.dat"
#define PRODUCTFILEPATH "D:\\FinalExam\\CodeNamePrice.dat"
#define ROOTDIRECTORYPATH "D:\\FinalExam\\"

//Struct definition for every existing product
struct Product
{
	int productCode;
	char productName[MAXSTRINGSIZE];
	double productPrice;

	int soldCount;
	double productRevenue;

	struct Product* nextListNode;   //Linked list link
};

//Struct definition for categories
struct Category
{
	int categoryCode;
	char categoryName[MAXSTRINGSIZE];

	int soldCount;
	double categoryRevenue;

	struct Product* productListHead;  //First element of the linked list
};

//Struct definiton for receipts formulated after processing the purchase
struct CustomerReceipt
{
	int customerID;
	char receiptContents[MAXFILESIZE];

	double receiptTotal;
	int receiptAmount;
};

void DefineCategories(struct Category*, const char*); //Intializes categories from file input
void DefineProducts(struct Category*, const char*);   //Initializes and links products with categories and each other via a linked list
struct CustomerReceipt* ProcessPurchase(struct Category*, const char*, int);   //Processes customer purchase, creates a sorted receipt
void SortPurchases(char[MAXRECEIPTSIZE][MAXSTRINGSIZE], char[MAXRECEIPTSIZE][MAXCODELENGTH], int);   //Bubble sorts the receipt by item codes 
void SwapLines(char*, char*, char*, char*); //Swaps lines inside bubble sort
char* FormatTotals(int, double);  //Manages total revenue/amount statistic formatting
void GenerateReports(struct Category*);  //Generates category reports and outputs into files and stdout 
void GenerateSummary(struct Category*, int);  //Generates daily summary and outputs into stdout
void CreateCategoryReport(struct Category*, char*);  //Handles report creation for specific category
char* StringGetHorizontalOffset(char*, int);   //Manages daily report totals formatting
void ListDeallocate(struct Product*);   //Deallocates linked list memory and links

int main()
{
	int customerCount = 0;
	int userInput;

	//Initialization
	struct Category* categoryArray = malloc(sizeof(struct Category) * CATEGORYCOUNT);
	const char* categoryFilePath = CATEGORYFILEPATH;
	const char* productFilePath = PRODUCTFILEPATH;

	DefineCategories(categoryArray, categoryFilePath);
	DefineProducts(categoryArray, productFilePath);

	//Main menu
	while (1)
	{
		printf("\n[1] - Serve customer\n[2] - Generate reports\n[3] - Generate daily summary\n[4] - Quit\n");
		scanf("%d", &userInput);

		if (userInput == 1)
		{
			char filePath[MAXSTRINGSIZE] = ROOTDIRECTORYPATH;
			char filePathExt[MAXSTRINGSIZE];


			printf("Enter transaction data filename: %s", ROOTDIRECTORYPATH);  //Prompts user to enter customer purchase data file inside defined root directory			                                                            //(an alternative to a barcode scanner)
			scanf(" %s", &filePathExt);

			strcat(filePath, filePathExt);

			customerCount++;
			struct CustomerReceipt* receipt;
			receipt = ProcessPurchase(categoryArray, filePath, customerCount);

			printf("%s", receipt->receiptContents);
		}
		if (userInput == 2)
		{
			GenerateReports(categoryArray);
		}
		if (userInput == 3)
		{
			GenerateSummary(categoryArray, customerCount);
		}
		if (userInput == 4)
			break;
	}

	ListDeallocate(categoryArray->productListHead);  //Deallocating linked list memory after user quits
	free(categoryArray);

	return 0;
}

//Traverses linked list until finds last element
struct Product* GetLastElement(struct Product* listHead)
{
	struct Product* currentNode = listHead;

	while (currentNode->nextListNode != NULL)
	{
		currentNode = currentNode->nextListNode;
	}

	return currentNode;
}

//Traverses linked list until product of specified key is found
struct Product* FindElementByKey(struct Product* listHead, int key)
{
	struct Product* currentNode = listHead;

	while (currentNode != NULL && currentNode->productCode != key)
	{
		currentNode = currentNode->nextListNode;
	}

	return currentNode;
}

//Traverses linked list by sprecified distance
struct Product* FindElementByIndex(struct Product* listHead, int index)
{
	struct Product* currentNode = listHead;

	int traverseCount = 0;

	while (currentNode != NULL)
	{
		if (traverseCount == index) return currentNode;

		currentNode = currentNode->nextListNode;
		traverseCount++;
	}

	return currentNode = NULL;
}

//Memory deallocation
void ListDeallocate(struct Product* listHead)
{
	struct Product* product = listHead;
	struct Product* productPrevious;

	while (product->nextListNode != NULL)
	{
		productPrevious = product;
		product = product->nextListNode;
		free(productPrevious);		
	}

	free(product);
}

//Checks whether a character is a space between words in order to correctly gather product and category names from file input
int IsASpaceSeparator(int index, char* string)
{
	if (string[index] != ' ') return 0;

	int surroundCheck = 0;

	for (int i = 1; i >= -1; i--)
	{
		if (index != 0 && ((string[index + i] >= 'A' && string[index + i] <= 'Z') || (string[index + i] >= 'a' && string[index + i] <= 'z'))) 
			surroundCheck++;
	}

	if (surroundCheck == 2)
		return 1;
	else
		return 0;
}

//Defines categories by scanning through the input file

//Since the provided input files formatted arguably poorly, it was necessary to come up with a workaround solution
//As such, this code gradually scans through the file, only picking up either numbers or letters, and concatting string character by character until end of line is detected
//When that happens, it dumps all of the gathered information into the category struct, effectively initializing it the category.

void DefineCategories(struct Category* categoryArray, const char* categoryFilepath)
{
	FILE* categoryNameFile;
	categoryNameFile = fopen(categoryFilepath, "r");

	char fileContent[MAXFILESIZE];

	fseek(categoryNameFile, 0L, SEEK_END); //This determines the length of file
	int fileSize = ftell(categoryNameFile);

	fseek(categoryNameFile, 0L, SEEK_SET);

	fread(fileContent, sizeof(char), fileSize, categoryNameFile);
	fclose(categoryNameFile);

	int linesRead = 0;

	char categoryCodeString[MAXCODELENGTH] = { "" };
	char categoryNameString[MAXSTRINGSIZE] = { "" };

	for (int i = 0; i < fileSize; i++)
	{	
		if (fileContent[i] >= '0' && fileContent[i] <= '9')
		{
			sprintf(categoryCodeString + strlen(categoryCodeString), "%c", fileContent[i]);
		}
		if (fileContent[i] >= 'a' && fileContent[i] <= 'z' || fileContent[i] >= 'A' && fileContent[i] <= 'Z' || IsASpaceSeparator(i, fileContent) == 1)
		{
			sprintf(categoryNameString + strlen(categoryNameString), "%c", fileContent[i]);
		}
		if (fileContent[i] == '\n')
		{
			sprintf(categoryCodeString + strlen(categoryCodeString), "\0");
			sprintf(categoryNameString + strlen(categoryNameString), "\0");

			categoryArray[linesRead].categoryCode = atoi(categoryCodeString);
			strcpy(categoryArray[linesRead].categoryName, categoryNameString);
			categoryArray[linesRead].categoryRevenue = 0;
			categoryArray[linesRead].soldCount = 0;
			categoryArray[linesRead].productListHead = NULL;

			linesRead++;

			memset(categoryNameString, '\0', MAXSTRINGSIZE);
			memset(categoryCodeString, '\0', MAXCODELENGTH);
		}
	}
}

//DefineProducts works very similarly to the method above

void DefineProducts(struct Category* categoryArray, const char* codeNameFilepath)
{
	FILE* codeNameFile;
	codeNameFile = fopen(codeNameFilepath, "r");

	char fileContent[MAXFILESIZE];

	fseek(codeNameFile, 0L, SEEK_END);
	int fileSize = ftell(codeNameFile);

	fseek(codeNameFile, 0L, SEEK_SET);

	fread(fileContent, sizeof(char), fileSize, codeNameFile);
	fclose(codeNameFile);

	int linesRead = 0;
	int isProductNameRead = 0;

	char productPriceString[MAXSTRINGSIZE] = { "" };
	char productCodeString[MAXCODELENGTH] = { "" };
	char productNameString[MAXSTRINGSIZE] = { "" };

	for (int i = 0; i < fileSize; i++)
	{
		if (isProductNameRead == 0 && fileContent[i] >= '0' && fileContent[i] <= '9')
		{
			sprintf(productCodeString + strlen(productCodeString), "%c", fileContent[i]);
		}
		if (fileContent[i] >= 'a' && fileContent[i] <= 'z' || fileContent[i] >= 'A' && fileContent[i] <= 'Z' || IsASpaceSeparator(i, fileContent) == 1)
		{
			sprintf(productNameString + strlen(productNameString), "%c", fileContent[i]);
			isProductNameRead = 1;
		}
		if (isProductNameRead == 1 && ((fileContent[i] >= '0' && fileContent[i] <= '9') || fileContent[i] == '.'))
		{
			sprintf(productPriceString + strlen(productPriceString), "%c", fileContent[i]);
		}
		if (fileContent[i] == '\n')
		{
			struct Product* newProductPtr = malloc(sizeof(struct Product));

			sprintf(productCodeString + strlen(productCodeString), "\0");
			sprintf(productNameString + strlen(productNameString), "\0");
			sprintf(productPriceString + strlen(productPriceString), "\0");

			newProductPtr->productCode = atoi(productCodeString);
			strcpy(newProductPtr->productName, productNameString);
			newProductPtr->productPrice = atof(productPriceString);
			newProductPtr->soldCount = 0;
			newProductPtr->productRevenue = 0;
			newProductPtr->nextListNode = NULL;

			int categoryKey = newProductPtr->productCode / 100 - 1;

			if (categoryArray[categoryKey].productListHead == NULL)
			{
				categoryArray[categoryKey].productListHead = newProductPtr;		

				int test = 1;
			}
			else
			{
				struct Product* lastListElement = GetLastElement(categoryArray[categoryKey].productListHead);
				lastListElement->nextListNode = newProductPtr;
			}

			memset(productNameString, '\0', MAXSTRINGSIZE);
			memset(productCodeString, '\0', MAXCODELENGTH);
			memset(productPriceString, '\0', MAXSTRINGSIZE);
			
			isProductNameRead = 0;
			linesRead++;
		}
	}
}

//CustomerReceipt method works much in the same way as the 2 methods earlier, with the main difference being the way infromation is passed into output

//Here, every gathered and calculated line is then collected in a 2d string array, together with the pre-separated item code.
//Those elements are then passed into the sort method, which utilizes simple bubble sort to reorder the strings.

struct CustomerReceipt* ProcessPurchase(struct Category* categoryArray, const char* purchaseFilepath, int customerCount)
{
	struct CustomerReceipt* ptrNewReceipt = malloc(sizeof(struct CustomerReceipt));
	sprintf(ptrNewReceipt->receiptContents, "Customer receipt #%d\nCode\t  Item name\t    Price       NumItem\t     TotSale\n", customerCount);
	ptrNewReceipt->receiptAmount = 0;
	ptrNewReceipt->receiptTotal = 0;

	FILE *purchasesFile;
	purchasesFile = fopen(purchaseFilepath, "r");

	char fileContent[MAXFILESIZE];

	fseek(purchasesFile, 0L, SEEK_END);
	int fileSize = ftell(purchasesFile);

	fseek(purchasesFile, 0L, SEEK_SET);

	fread(fileContent, sizeof(char), fileSize, purchasesFile);
	fclose(purchasesFile);

	int linesRead = 0;
	int isCodeRead = 0;
	
	char productAmountString[MAXSTRINGSIZE] = { "" };
	char productCodes[MAXRECEIPTSIZE][MAXCODELENGTH] = { "" };
	char receiptLines[MAXRECEIPTSIZE][MAXSTRINGSIZE] = { "" };

	for (int i = 0; i < fileSize; i++)
	{
		if (fileContent[i] >= '0' && fileContent[i] <= '9' && isCodeRead == 0)
		{
			sprintf(productCodes[linesRead] + strlen(productCodes[linesRead]), "%c", fileContent[i]);
		}
		if (fileContent[i] >= '0' && fileContent[i] <= '9' && isCodeRead == 1)
		{
			sprintf(productAmountString + strlen(productAmountString), "%c", fileContent[i]);
		}

		if (strlen(productCodes[linesRead]) == MAXCODELENGTH - 1) isCodeRead = 1;

		if (productCodes[linesRead][0] == '0')
		{
			sprintf(ptrNewReceipt->receiptContents + strlen(ptrNewReceipt->receiptContents), "\0");
			break;
		}

		if (fileContent[i] == '\n')
		{
			int productCode = atoi(productCodes[linesRead]);
			int categoryCode = (productCode / 100) - 1;

			int productAmount = atoi(productAmountString);

			sprintf(productCodes[linesRead] +strlen(productCodes[linesRead]), "\0");
			sprintf(productAmountString + strlen(productAmountString), "\0");

			struct Product* updatedProduct = FindElementByKey(categoryArray[categoryCode].productListHead, productCode);
			updatedProduct->soldCount += productAmount;
			updatedProduct->productRevenue += productAmount * updatedProduct->productPrice;

			categoryArray[categoryCode].soldCount += productAmount;
			categoryArray[categoryCode].categoryRevenue += productAmount * updatedProduct->productPrice;

			int contentCharIndex = strlen(ptrNewReceipt->receiptContents);			

			int lineSpacing = 0;


			//Code structures similar to this one thoutghout the code are responsible for pleasant-looking formatting of output.
			//In essence, it gradually constructs the output string using predetermined column indexes as "flags" for when a new column of data needs to be displayed.

			while (lineSpacing <= 70) 
			{
				int lineAdvance = strlen(receiptLines[linesRead]);

				switch (lineSpacing)
				{
					case 1:						
						sprintf(receiptLines[linesRead] + lineSpacing, "%d", updatedProduct->productCode);
						lineSpacing += strlen(receiptLines[linesRead]) - lineAdvance;
						break;
					case 10:
						sprintf(receiptLines[linesRead] + lineSpacing, "%s", updatedProduct->productName);
						lineSpacing += strlen(receiptLines[linesRead]) - lineAdvance;
						break;
					case 28:
						sprintf(receiptLines[linesRead] + lineSpacing, "%.2f", updatedProduct->productPrice);
						lineSpacing += strlen(receiptLines[linesRead]) - lineAdvance;
						break;
					case 43:
						sprintf(receiptLines[linesRead] + lineSpacing, "%d", productAmount);
						lineSpacing += strlen(receiptLines[linesRead]) - lineAdvance;
						break;
					case 53:
						sprintf(receiptLines[linesRead] + lineSpacing, "%.2f\n\0", updatedProduct->productPrice * productAmount);
						lineSpacing += strlen(receiptLines[linesRead]);
						break;
					default:
						sprintf(receiptLines[linesRead] + strlen(receiptLines[linesRead]), " ");
						lineSpacing++;
				}
			}

			ptrNewReceipt->receiptTotal += productAmount * updatedProduct->productPrice;
			ptrNewReceipt->receiptAmount += productAmount;

			memset(productAmountString, '\0', MAXSTRINGSIZE);

			linesRead++;
			isCodeRead = 0;
		}
	}

	SortPurchases(receiptLines, productCodes, linesRead); //Sorting
	
	char output[MAXFILESIZE] = { "" };

	for (int i = 0; i < linesRead; i++)
	{
		sprintf(output + strlen(output), receiptLines[i]);
	}

	char total[MAXSTRINGSIZE];
	strcpy(total, FormatTotals(ptrNewReceipt->receiptAmount, ptrNewReceipt->receiptTotal));
	sprintf(output + strlen(output), "\n%s", total);

	strcat(ptrNewReceipt->receiptContents, output);

	return ptrNewReceipt;
}

//This basically utilizes the formatting structure above to make total revenue strings look good.
char* FormatTotals(int totalNum, double totalCash)
{
	int lineSpacing = 0;
	char totalsLine[MAXSTRINGSIZE] = { "" };

	while (lineSpacing <= 54)
	{
		int lineAdvance = strlen(totalsLine);

		switch (lineSpacing)
		{
		case 35:
			sprintf(totalsLine + lineSpacing, "Totals: %d", totalNum);
			lineSpacing += strlen(totalsLine) - lineAdvance;
			break;
		case 53:
			sprintf(totalsLine + lineSpacing, "%.2f\0", totalCash);
			lineSpacing += strlen(totalsLine);
			break;
		default:
			sprintf(totalsLine + strlen(totalsLine), " ");
			lineSpacing++;
		}
	}

	return totalsLine;
}

//Bubble sort
void SortPurchases(char receiptLines[MAXRECEIPTSIZE][MAXSTRINGSIZE], char productCodes[MAXRECEIPTSIZE][MAXCODELENGTH], int lineCount)
{
	for (int i = 0; i < lineCount - 1; i++)
	{
		for (int j = 0; j < lineCount - i - 1; j++)
		{
			if (atoi(productCodes[j]) > atoi(productCodes[j + 1]))
			{
				SwapLines(receiptLines[j], receiptLines[j + 1], productCodes[j], productCodes[j + 1]);
			}
		}
	}
}

void SwapLines(char* line1, char* line2, char* code1, char* code2)
{
	char temp1[MAXSTRINGSIZE] = { "" };

	strcpy(temp1, line1);
	strcpy(line1, line2);
	strcpy(line2, temp1);

	char temp2[MAXCODELENGTH] = { "" };

	strcpy(temp2, code1);
	strcpy(code1, code2);
	strcpy(code2, temp2);
}

void GenerateReports(struct Category* categoryArray)
{
	for (int i = 0; i < CATEGORYCOUNT; i++)
	{
		CreateCategoryReport(categoryArray + i, ROOTDIRECTORYPATH);
	}

}

void GenerateSummary(struct Category* categoryArray, int customerCount)
{
	char output[MAXFILESIZE] = { "\nDaily Summary Report\nCode  \t  Category Name  \t Items Sold  \t  Tot Sales Amt\n" };

	int amountSold = 0;
	float totalRevenue = 0;

	for (int i = 0; i < CATEGORYCOUNT; i++)
	{
		int lineSpacing = 0;
		int headerTextSize = strlen(output);

		while (lineSpacing <= 55)
		{
			int lineAdvance = strlen(output);

			switch (lineSpacing)
			{
			case 1:
				sprintf(output + headerTextSize + lineSpacing, "%d", (categoryArray + i)->categoryCode);
				lineSpacing += strlen(output) - lineAdvance;
				break;
			case 10:
				sprintf(output + headerTextSize + lineSpacing, "%s", (categoryArray + i)->categoryName);
				lineSpacing += strlen(output) - lineAdvance;
				break;
			case 37:
				sprintf(output + headerTextSize + lineSpacing, "%d", (categoryArray + i)->soldCount);
				lineSpacing += strlen(output) - lineAdvance;
				break;
			case 50:
				sprintf(output + headerTextSize + lineSpacing, "%.2f\n\n", (categoryArray + i)->categoryRevenue);
				lineSpacing += strlen(output);
				break;
			default:
				sprintf(output + strlen(output), " ");
				lineSpacing++;
			}
		}

		amountSold += (categoryArray + i)->soldCount;
		totalRevenue += (categoryArray + i)->categoryRevenue;
	}

	char totals[MAXSTRINGSIZE];           //Here, space offsets are created depending on the length of the characters to the left of displayed data
	char horOffset1[32] = { "" };         //This conversely makes it so that that data appears in a uniform column
	char horOffset2[32] = { "" };
	char horOffset3[32] = { "" };

	strcpy(horOffset1, StringGetHorizontalOffset("Total customers: ", 25));
	strcpy(horOffset2, StringGetHorizontalOffset("Total Items Sold: ", 25));
	strcpy(horOffset3, StringGetHorizontalOffset("Total Sales: ", 25));

	sprintf(totals, "Total customers: %s%d\nTotal Items Sold: %s%d\nTotal Sales: %s%.2f\n\0",
		horOffset1, customerCount, horOffset2, amountSold,
		horOffset3, totalRevenue);

	strcat(output, totals);

	printf("%s", output);
}

//Formulates space offset strings
char* StringGetHorizontalOffset(char* originString, int fixedOffsetWidth)
{
	char horizontalOffset[32] = { "" };

	for (int i = strlen(originString); i < fixedOffsetWidth; i++)
	{
		sprintf(horizontalOffset + strlen(horizontalOffset), " ");
	}

	return horizontalOffset;
}

//Uses much of the same logic as everything else, except now everything is also outputted into report files.
//Filenames are concatted as requested using category codes

void CreateCategoryReport(struct Category* category, char* categoryRootDirectory)
{
	int i = 0;
	int areProductsLeft = 1;

	char fileOutput[MAXFILESIZE] = { "" };
	char fileOutputMid[MAXFILESIZE] = { "" };
	double totalCash = 0;
	int totalAmount = 0;

	sprintf(fileOutput, "Category name: %s\nCategory code: %d\n Code\t Item Name\t\t NumItems\t TotSale\n", category->categoryName, category->categoryCode);

	while (areProductsLeft)
	{
		struct Product* product = FindElementByIndex(category->productListHead, i);
		
		if (product == NULL)
		{
			areProductsLeft = 0;
			break;
		}

		totalAmount += product->soldCount;
		totalCash += product->productRevenue;

		int lineSpacing = 0;
		int headerTextSize = strlen(fileOutputMid);

		while (lineSpacing <= 55)
		{
			int lineAdvance = strlen(fileOutputMid);

			switch (lineSpacing)
			{
			case 1:
				sprintf(fileOutputMid + headerTextSize + lineSpacing, "%d", product->productCode);
				lineSpacing += strlen(fileOutputMid) - lineAdvance;
				break;
			case 10:
				sprintf(fileOutputMid + headerTextSize + lineSpacing, "%s", product->productName);
				lineSpacing += strlen(fileOutputMid) - lineAdvance;
				break;
			case 37:
				sprintf(fileOutputMid + headerTextSize + lineSpacing, "%d", product->soldCount);
				lineSpacing += strlen(fileOutputMid) - lineAdvance;
				break;
			case 50:
				sprintf(fileOutputMid + headerTextSize + lineSpacing, "%.2f\n", product->productRevenue);
				lineSpacing += strlen(fileOutputMid);
				break;
			default:
				sprintf(fileOutputMid + strlen(fileOutputMid), " ");
				lineSpacing++;
			}
		}

		i++;
	}

	printf("\n\n%s", fileOutputMid);

	char totals[MAXSTRINGSIZE];
	strcpy(totals, FormatTotals(totalAmount, totalCash));

	strcat(fileOutput, fileOutputMid);
	strcat(fileOutput, totals);

	FILE* categoryReportFile;

	char categoryFileName[MAXROOTLENGTH] = { ROOTDIRECTORYPATH };
	sprintf(categoryFileName + strlen(categoryFileName), "Inventory%d\.dat", category->categoryCode);
	categoryReportFile = fopen(categoryFileName, "w");

	fwrite(fileOutput, sizeof(char), strlen(fileOutput), categoryReportFile);
	fclose(categoryReportFile);
}