
#include<iostream>
using namespace std;
int vis[100000], d[10000], ans = 1, n, k;
char str[100000];
void dfs(int x, int cnt, int sum)
{
	if (cnt == n - 1)
	{
		printf("%d\n", sum);
		return;
	}
	for (int i = 0; i < k; i++)
	{
		if (vis[i] == 0)
		{
			vis[i] = 1;
			dfs(d[i], cnt + 1, sum * 10 + d[i]);
			vis[i] = 0;
		}
	}
	return;
}
int main()
{
	memset(vis, 0, sizeof(vis));
	while (cin >> str)
	{
		k = strlen(str);
		cin >> n;
		for (int i = 0; i < k; i++)
			d[i] = str[i] - '0';
		for (int i = 0; i < k; i++)
		{
			ans = 0;
			vis[i] = 1;
			dfs(d[i], ans, d[i]);
			vis[i] = 0;
		}

	}
	return 0;
}