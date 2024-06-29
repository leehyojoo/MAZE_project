/*

	ofxWinMenu basic example - ofApp.cpp

	Example of using ofxWinMenu addon to create a menu for a Microsoft Windows application.

	Copyright (C) 2016-2017 Lynn Jarvis.

	https://github.com/leadedge

	http://www.spout.zeal.co

	=========================================================================
	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU Lesser General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU Lesser General Public License for more details.

	You should have received a copy of the GNU Lesser General Public License
	along with this program.  If not, see <http://www.gnu.org/licenses/>.
	=========================================================================

	03.11.16 - minor comment cleanup
	21.02.17 - rebuild for OF 0.9.8

*/
#include "ofApp.h"
#include <iostream>
using namespace std;
//--------------------------------------------------------------
void ofApp::setup() {

	ofSetWindowTitle("Maze Example"); // Set the app name on the title bar
	ofSetFrameRate(15);
	ofBackground(255, 255, 255);
	// Get the window size for image loading
	windowWidth = ofGetWidth();
	windowHeight = ofGetHeight();
	isdfs = false;
	isOpen = 0;
	// Centre on the screen
	ofSetWindowPosition((ofGetScreenWidth() - windowWidth) / 2, (ofGetScreenHeight() - windowHeight) / 2);

	// Load a font rather than the default
	myFont.loadFont("verdana.ttf", 12, true, true);

	// Load an image for the example
	//myImage.loadImage("lighthouse.jpg");

	// Window handle used for topmost function
	hWnd = WindowFromDC(wglGetCurrentDC());

	// Disable escape key exit so we can exit fullscreen with Escape (see keyPressed)
	ofSetEscapeQuitsApp(false);

	//
	// Create a menu using ofxWinMenu
	//

	// A new menu object with a pointer to this class
	menu = new ofxWinMenu(this, hWnd);

	// Register an ofApp function that is called when a menu item is selected.
	// The function can be called anything but must exist. 
	// See the example "appMenuFunction".
	menu->CreateMenuFunction(&ofApp::appMenuFunction);

	// Create a window menu
	HMENU hMenu = menu->CreateWindowMenu();

	//
	// Create a "File" popup menu
	//
	HMENU hPopup = menu->AddPopupMenu(hMenu, "File");

	//
	// Add popup items to the File menu
	//

	// Open an maze file
	menu->AddPopupItem(hPopup, "Open", false, false); // Not checked and not auto-checked

	// Final File popup menu item is "Exit" - add a separator before it
	menu->AddPopupSeparator(hPopup);
	menu->AddPopupItem(hPopup, "Exit", false, false);

	//
	// View popup menu
	//
	hPopup = menu->AddPopupMenu(hMenu, "View");

	bShowInfo = true;  // screen info display on
	menu->AddPopupItem(hPopup, "Show DFS", false, false); // Checked
	bTopmost = false; // app is topmost
	menu->AddPopupItem(hPopup, "Show BFS"); // Not checked (default)
	bFullscreen = false; // not fullscreen yet
	menu->AddPopupItem(hPopup, "Full screen", false, false); // Not checked and not auto-check

	//
	// Help popup menu
	//
	hPopup = menu->AddPopupMenu(hMenu, "Help");
	menu->AddPopupItem(hPopup, "About", false, false); // No auto check

	// Set the menu to the window
	menu->SetWindowMenu();

} // end Setup


//
// Menu function
//
// This function is called by ofxWinMenu when an item is selected.
// The the title and state can be checked for required action.
// 
void ofApp::appMenuFunction(string title, bool bChecked) {

	ofFileDialogResult result;
	string filePath;
	size_t pos;

	//
	// File menu
	//
	if (title == "Open") {
		readFile();
	}
	if (title == "Exit") {
		ofExit(); // Quit the application
	}

	//
	// Window menu
	//
	if (title == "Show DFS") {
		//bShowInfo = bChecked;  // Flag is used elsewhere in Draw()
		if (isOpen)
		{
			DFS();
			bShowInfo = bChecked;
		}
		else
			cout << "you must open file first" << endl;

	}

	if (title == "Show BFS") {
		doTopmost(bChecked); // Use the checked value directly

	}

	if (title == "Full screen") {
		bFullscreen = !bFullscreen; // Not auto-checked and also used in the keyPressed function
		doFullScreen(bFullscreen); // But als take action immediately
	}

	//
	// Help menu
	//
	if (title == "About") {
		ofSystemAlertDialog("ofxWinMenu\nbasic example\n\nhttp://spout.zeal.co");
	}

} // end appMenuFunction


//--------------------------------------------------------------
void ofApp::update() {

}


//--------------------------------------------------------------
void ofApp::draw() {
	char str[256];
	ofSetColor(100);
	ofSetLineWidth(5);
	int i, j;

	// Draw the maze if isOpen flag is true
	if (isOpen) {
		int maze_size = 30;

		// Loop through the maze array and draw walls accordingly
		for (int i = 0; i < HEIGHT; i++) {
			for (int j = 0; j < WIDTH; j++) {
				if (maze[i * WIDTH + j] & WALL_UP)
					ofDrawLine(j * maze_size, i * maze_size, (j + 1) * maze_size, i * maze_size);
				if (maze[i * WIDTH + j] & WALL_DOWN)
					ofDrawLine(j * maze_size, (i + 1) * maze_size, (j + 1) * maze_size, (i + 1) * maze_size);
				if (maze[i * WIDTH + j] & WALL_LEFT)
					ofDrawLine(j * maze_size, i * maze_size, j * maze_size, (i + 1) * maze_size);
				if (maze[i * WIDTH + j] & WALL_RIGHT)
					ofDrawLine((j + 1) * maze_size, i * maze_size, (j + 1) * maze_size, (i + 1) * maze_size);
			}
		}
	}

	// If isdfs flag is true, draw using depth-first search algorithm
	if (isdfs) {
		ofSetColor(200);
		ofSetLineWidth(5);
		if (isOpen)
			dfsdraw();
		else
			cout << "���� ���� ����" << endl;
	}

	// Show additional information if bShowInfo flag is true
	if (bShowInfo) {
		// Show title
		sprintf(str, "����");
		myFont.drawString(str, 15, ofGetHeight() - 20);
	}
} // end Draw


void ofApp::doFullScreen(bool bFull)
{
	// Enter full screen
	if (bFull) {
		// Remove the menu but don't destroy it
		menu->RemoveWindowMenu();
		// hide the cursor
		ofHideCursor();
		// Set full screen
		ofSetFullscreen(true);
	}
	else {
		// return from full screen
		ofSetFullscreen(false);
		// Restore the menu
		menu->SetWindowMenu();
		// Restore the window size allowing for the menu
		ofSetWindowShape(windowWidth, windowHeight + GetSystemMetrics(SM_CYMENU));
		// Centre on the screen
		ofSetWindowPosition((ofGetScreenWidth() - ofGetWidth()) / 2, (ofGetScreenHeight() - ofGetHeight()) / 2);
		// Show the cursor again
		ofShowCursor();
		// Restore topmost state
		if (bTopmost) doTopmost(true);
	}

} // end doFullScreen


void ofApp::doTopmost(bool bTop)
{
	if (bTop) {
		// get the current top window for return
		hWndForeground = GetForegroundWindow();
		// Set this window topmost
		SetWindowPos(hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
		ShowWindow(hWnd, SW_SHOW);
	}
	else {
		SetWindowPos(hWnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
		ShowWindow(hWnd, SW_SHOW);
		// Reset the window that was topmost before
		if (GetWindowLong(hWndForeground, GWL_EXSTYLE) & WS_EX_TOPMOST)
			SetWindowPos(hWndForeground, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
		else
			SetWindowPos(hWndForeground, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
	}
} // end doTopmost


//--------------------------------------------------------------
void ofApp::keyPressed(int key) {

	// Escape key exit has been disabled but it can be checked here
	if (key == VK_ESCAPE) {
		// Disable fullscreen set, otherwise quit the application as usual
		if (bFullscreen) {
			bFullscreen = false;
			doFullScreen(false);
		}
		else {
			ofExit();
		}
	}

	// Remove or show screen info
	if (key == ' ') {
		bShowInfo = !bShowInfo;
		// Update the menu check mark because the item state has been changed here
		menu->SetPopupItem("Show DFS", bShowInfo);
	}

	if (key == 'f') {
		bFullscreen = !bFullscreen;
		doFullScreen(bFullscreen);
		// Do not check this menu item
		// If there is no menu when you call the SetPopupItem function it will crash
	}

} // end keyPressed

//--------------------------------------------------------------
void ofApp::keyReleased(int key) {

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y) {

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button) {

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button) {

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button) {

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h) {

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg) {

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo) {

}

bool ofApp::readFile() {
	// .maz ���� ������ ���� ���� ��ȭ���� ����
	ofFileDialogResult openFileResult = ofSystemLoadDialog("Select .maz file");
	string filePath;
	size_t pos;

	// ����ڰ� ���� ���ÿ� ������ ��� Ȯ��
	if (openFileResult.bSuccess) {
		ofLogVerbose("����ڰ� ������ �����߽��ϴ�.");

		// ���� ���� ����
		string fileName = openFileResult.getName();
		filePath = openFileResult.getPath();
		printf("���� �̸�: %s\n", fileName.c_str());

		// ������ .maz Ȯ���ڸ� ������ �ִ��� Ȯ��
		pos = filePath.find_last_of(".");
		if (pos != string::npos && pos != 0 && filePath.substr(pos + 1) == "maz") {
			// ������ �����ϴ��� Ȯ��
			ofFile file(fileName);
			if (!file.exists()) {
				cout << "��� ������ �������� �ʽ��ϴ�." << endl;
				return false;
			}
			else {
				cout << "��� ������ ã�ҽ��ϴ�." << endl;
				isOpen = true;
			}

			// ���ۿ� ���� �б�
			ofBuffer buffer(file);

			// ���ۿ��� ���� ó��
			vector<string> lines;
			for (auto line : buffer.getLines()) {
				lines.push_back(line);
			}

			// �̷� ũ�� ����
			HEIGHT = (lines.size() - 1) / 2;
			WIDTH = (lines[0].size() - 1) / 2;

			// �Է°� �̷� �迭�� ���� �޸� �Ҵ�
			input = (char**)malloc(sizeof(char*) * lines.size());
			maze = (int*)malloc(sizeof(int) * HEIGHT * WIDTH);
			memset(maze, 0, sizeof(int) * HEIGHT * WIDTH);

			// �Է� �迭 ä���
			for (int i = 0; i < lines.size(); i++) {
				input[i] = (char*)malloc(sizeof(char) * lines[i].size());
				strcpy(input[i], lines[i].c_str());
			}

			// �̷� �� ���� �м�
			for (int i = 0; i < HEIGHT; i++) {
				for (int j = 0; j < WIDTH; j++) {
					int y = 1 + i * 2;
					int x = 1 + j * 2;

					if (input[y - 1][x] == '-') maze[i * WIDTH + j] |= WALL_UP;
					if (input[y + 1][x] == '-') maze[i * WIDTH + j] |= WALL_DOWN;
					if (input[y][x - 1] == '|') maze[i * WIDTH + j] |= WALL_LEFT;
					if (input[y][x + 1] == '|') maze[i * WIDTH + j] |= WALL_RIGHT;
				}
			}

			isOpen = true;
			return true;
		}
		else {
			printf(".maz Ȯ���ڸ� ����ؾ� �մϴ�.\n");
			return false;
		}
	}
	else {
		// ���� ��ȭ���� ��� �Ǵ� ����
		return false;
	}
}

void ofApp::freeMemory() {

	for (int i = 0; i < HEIGHT * 2 + 1; i++) {
		free(input[i]);
	}

	free(input);
	free(maze);
}

bool ofApp::DFS()
{
	return 1;
}
void ofApp::dfsdraw()
{
}