//implement a Distance Vector Routing using Bellman-Ford algorithm

#include <stdio.h>
#include <stdlib.h>

const int INF = 99999;

void bellman_ford(int source, int n, int m, int adj[100][100])
{
	//we have to calculate the shortest path from source to all other nodes
	//also we have to maintain the next hop to reach the destination from source
	int dist[100];
	int next[100];
	int parent[100];
	int i, j, k;
	//setting all distances to infinity
	for(i = 1; i <= n; i++) dist[i] = INF, next[i] = -1, parent[i] = -1;
	//distance from source to source is 0
	//next hop is itself
	dist[source] = 0;
	next[source] = source;

	for(i = 0; i < n-1; ++i) {
		for(j = 1; j <= n; ++j) {
			for(k = 1; k <= n; ++k) {
				if(adj[j][k] == INF) continue;
				//if there is a path from j to k
				//and the distance from j to k is less than the current distance
				//then update the distance and next hop
				//j -> k
				if(dist[j] + adj[j][k] < dist[k]) {
					dist[k] = dist[j] + adj[j][k];
					parent[k] = j;
				}
			}
		}
	}

	//check for negative weight cycles
	for(j = 1; j <= n; ++j) {
		for(k = 1; k <= n; ++k) {
			if(adj[j][k] == INF) continue;
			if(dist[j] + adj[j][k] < dist[k]) {
				printf("Negative weight cycle exists\n");
				exit(0);
			}
		}
	}

	//we have to find the next hop to reach the destination from source
	for(i = 1; i <= n; ++i) {
		//we will find the next hope to reach i from the source
		if(parent[i] == -1) next[i] = i;
		else {
			j = i;
			while(parent[j] != source) {
				j = parent[j];
			}
			next[i] = j;
		}
	}

	//now we have the shortest path from source to all other nodes
	//and the next hop to reach the destination from source
	//print the Distance Vector Routing Table
	for(int i = 1; i <= n; ++i) {
		//print destination, parent and distance
		printf("%d %d %d\n", i, next[i], dist[i]);
	}
}
int main()
{
	int n, m;
	int adj[100][100];
	for(int i = 0; i < 100; i++) {
		for(int j = 0; j < 100; j++) {
			adj[i][j] = INF;
		}
	}
	scanf("%d %d", &n, &m);
	for (int i = 0; i < m; i++) {
		int u, v, w;
		scanf("%d %d %d", &u, &v, &w);
		adj[u][v] = w;
		adj[v][u] = w;
	}
	for(int i = 1; i <= n; i++) {
		fprintf(stderr, "%d\n", i);
		bellman_ford(i, n, m, adj);
		fprintf(stderr, "------------------------\n");
	}
	return 0;
}
