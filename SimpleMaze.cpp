#include<bits/stdc++.h>
using namespace std;
#include<GL/glut.h>

int dx[4] = { 0,0,1,-1 };
int dy[4] = { 1,-1,0,0 };

void setColor(double r, double g, double b) { glColor3d(r, g, b); }

vector<string>MAZE;
vector<vector<int>>Dists;

int mxX, mnX, mnY, mxY;
int goalX, goalY, goalDist;
int posX, posY;
bool gameOver;

void genMaze(vector<string>& G) {
	srand(time(0));
	G = vector<string>(mxX, string(mxY, '#'));
	for (int i = 1;i < mxX;i += 2) {
		for (int j = 1;j < mxY;j += 2) {
			G[i][j] = ' ';
		}
	}
	vector<vector<bool>>vis(mxX, vector<bool>(mxY, 0));
	for (int i = 1;i < mxX;i += 2) {
		for (int j = 1;j < mxY;j += 2) {
			int len = 5 + rand() % (mxX / 2);
			int px = i, py = j;
			vis[px][py] = 1;
			while (len--) {
				int d = rand() % 4;
				int nx = px + dx[d] * 2;
				int ny = py + dy[d] * 2;
				if (nx < 0 || ny < 0)continue;
				if (nx >= mxX - 1 || ny >= mxY - 1)continue;
				if (vis[nx][ny])continue;
				G[px + dx[d]][py + dy[d]] = ' ';
				vis[nx][ny] = 1;
				px = nx;
				py = ny;
			}
		}
	}
}

void quad(int x, int y, int h, int w) {
	glBegin(GL_POLYGON);
	glVertex2d(x, y);
	glVertex2d(x + w, y);
	glVertex2d(x + w, y + h);
	glVertex2d(x, y + h);
	glEnd();
}

void setGoal() {
	vector<vector<bool>>vis = vector<vector<bool>>(mxX, vector<bool>(mxY, 0));
	Dists = vector<vector<int>>(mxX, vector<int>(mxY, INT_MAX));
	queue<pair<pair<int, int>, int>>Q;
	Q.push({ {1,1},0 });
	vis[1][1] = 1;
	while (!Q.empty()) {
		auto [pos, dist] = Q.front();
		Q.pop();
		auto [x, y] = pos;
		Dists[x][y] = min(Dists[x][y], dist);
		if (dist > goalDist) {
			goalDist = dist;
			goalX = x;
			goalY = y;
		}
		for (int i = 0;i < 4;i++) {
			int nx = x + dx[i] * 2;
			int ny = y + dy[i] * 2;
			if (nx < 0 || ny < 0)continue;
			if (nx >= mxX - 1 || ny >= mxY - 1)continue;
			if (vis[nx][ny])continue;
			if (MAZE[x + dx[i]][y + dy[i]] == '#')continue;
			Q.push({ {nx,ny},dist + 1 });
			vis[nx][ny] = 1;
		}
	}
}

void markShortestPaths(int x, int y) {
	gameOver = 1;
	if (!x && !y)return;
	MAZE[x][y] = '+';
	for (int i = 0;i < 4;i++) {
		int nx = x + dx[i] * 2;
		int ny = y + dy[i] * 2;
		if (nx < 0 || ny < 0)continue;
		if (nx >= mxX - 1 || ny >= mxY - 1)continue;
		if (MAZE[x + dx[i]][y + dy[i]] == '#')continue;
		if (Dists[nx][ny] != Dists[x][y] - 1)continue;
		MAZE[x + dx[i]][y + dy[i]] = '+';
		markShortestPaths(nx, ny);
	}
}

void input() {
	int n = 30;
	mxX = 2 * n + 1;
	mxY = mxX;
}

void valueInit() {
	genMaze(MAZE);
	goalX = 0;
	goalY = 0;
	goalDist = 0;
	setGoal();
	posX = 1;
	posY = 1;
	MAZE[1][1] = '*';
	gameOver = 0;
	if (goalDist < max(mxX, mxY))valueInit();
}

void windowInit() {
	glutInitWindowSize(500, 600);
	glutInitWindowPosition(0, 0);
	glutCreateWindow("Maze");
	glClearColor(0.0, 0.0, 0.0, 1.0);
	glOrtho(0, 2 * mxX, -mxY / 2, 2 * mxY, -1, 1);
}

void fillReachable(int x, int y) {
	if (gameOver)return;
	MAZE[x][y] = 'X';
	for (int i = 0;i < 4;i++) {
		int nx = x + dx[i] * 2;
		int ny = y + dy[i] * 2;
		if (nx < 0 || ny < 0)continue;
		if (nx >= mxX - 1 || ny >= mxY - 1)continue;
		if (MAZE[nx][ny] == 'X')continue;
		if (MAZE[x + dx[i]][y + dy[i]] == '#')continue;
		MAZE[x + dx[i]][y + dy[i]] = 'X';
		fillReachable(nx, ny);
	}
}

void placePath(int i, int j) {
	if (i % 2 && j % 2)quad(i * 2 - 1, j * 2 - 1, 3, 3);
	else if (i % 2 == 0 && j % 2 == 0)quad(i * 2, j * 2, 1, 1);
	else if (i % 2)quad(i * 2 - 1, j * 2, 1, 3);
	else if (j % 2)quad(i * 2, j * 2 - 1, 3, 1);
}

void drawMaze() {
	for (int i = 0;i < mxX;i++) {
		for (int j = 0;j < mxY;j++) {
			if (MAZE[i][j] == '#')setColor(1.0, 1.0, 1.0), placePath(i, j);
			if (MAZE[i][j] == '+')setColor(0.21484375, 0.72265625, 0.2109375), placePath(i, j);
			if (MAZE[i][j] == '*')setColor(1.0, 0, 0), placePath(i, j);
			if (MAZE[i][j] == 'X')setColor(1.0, 0.5, 0.5), placePath(i, j);
		}
	}
	setColor(0, 1.0, 0);
	quad(2 * goalX - 1, 2 * goalY - 1, 3, 3);
	setColor(0, 0, 1.0);
	quad(2 * posX - 1, 2 * posY - 1, 3, 3);
}

void textLine(string s, int y) {
	glRasterPos2i(10, -y * 5 - 2);
	void* font = GLUT_BITMAP_TIMES_ROMAN_24;
	for (auto x : s) {
		glutBitmapCharacter(font, x);
	}
}

void textInstructions() {
	if (!gameOver) {
		setColor(1.0, 1.0, 1.0);
		textLine("Press", 1);
		setColor(1.0, 0.2, 0.2);
		textLine("\t\tR to restart", 2);
		setColor(0.2, 0.8, 1.0);
		textLine("\t\tF to see reachable cells", 3);
		setColor(0.0, 1.0, 0.0);
		textLine("\t\tS to see the solution", 4);
	}
	else {
		setColor(1.0, 1.0, 1.0);
		textLine("Press", 2);
		setColor(1.0, 0.2, 0.2);
		textLine("     R to restart", 3);
	}
}

void display() {
	glClear(GL_COLOR_BUFFER_BIT);
	glPushMatrix();
	drawMaze();
	textInstructions();
	glPopMatrix();
	glFlush();
}

void move(int d) {
	if (gameOver)return;
	int nx = posX + dx[d] * 2;
	int ny = posY + dy[d] * 2;
	if (nx < 0 || ny < 0)return;
	if (nx >= mxX - 1 || ny >= mxY - 1)return;
	if (MAZE[posX + dx[d]][posY + dy[d]] == '#')return;
	MAZE[posX + dx[d]][posY + dy[d]] = '*';
	MAZE[nx][ny] = '*';
	posX = nx;
	posY = ny;
	if (posX == goalX && posY == goalY)markShortestPaths(goalX, goalY);
}

void spe_key(int key, int x, int y) {
	int dir;
	if (key == GLUT_KEY_LEFT)dir = 3;
	else if (key == GLUT_KEY_RIGHT)dir = 2;
	else if (key == GLUT_KEY_UP)dir = 0;
	else if (key == GLUT_KEY_DOWN)dir = 1;
	move(dir);
	glutPostRedisplay();
}

void func_key(unsigned char key, int x, int y) {
	if (key == 'r')valueInit();
	if (key == 'f')fillReachable(1, 1);
	if (key == 's')markShortestPaths(goalX, goalY);
	glutPostRedisplay();
}


int main(int argc, char** argv) {
	glutInit(&argc, argv);
	input();
	valueInit();
	windowInit();
	glutDisplayFunc(display);
	glutSpecialFunc(spe_key);
	glutKeyboardFunc(func_key);
	glutMainLoop();
	return 0;
}
