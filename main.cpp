#include <iostream>
#include <memory>
#include <windows.h>

#include "KuskApp.h"

using namespace std;

int main() {
	kusk::KuskApp kuskApp;

	if (!kuskApp.Initialize()) {
		cout << "Initialization failed." << endl;
		return -1;
	}

	return kuskApp.Run();
}