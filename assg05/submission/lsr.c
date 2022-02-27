// implement Dijkstra's algorithm for shortest path to illustrate Link State Routing

#include <stdio.h>

const int INF = 99999;
void dijkstra(int adj[100][100], int n, int source) {
	int dist[100];
	int prev[100];
	int visited[100];
	int i, u, v, w;
	//initialize distance to INF and prev to -1 and not visited
	for (i = 1; i <= n; i++) {
		dist[i] = INF;
		prev[i] = -1;
		visited[i] = 0;
	}
	dist[source] = 0;
	//finding shortest path
	for (i = 1; i <= n; i++) {
		int min = INF;
		//finding the node with minimum distance from source
		//for the first time this will be source
		for (u = 1; u <= n; u++) {
			if (!visited[u] && dist[u] < min) {
				min = dist[u];
				v = u;
			}
		}
		if(min == INF) break;
		//v has the node with minimum distance from the source
		visited[v] = 1;
		for (w = 1; w <= n; w++) {
			//checking if the node is not visited and the distance from source is less than the distance from v
			//edge is from v to w
			if (!visited[w] && adj[v][w] != INF && dist[v] + adj[v][w] < dist[w]) {
				dist[w] = dist[v] + adj[v][w];
				prev[w] = v;
			}
		}
	}
	//printing the shortest path and path length
	for (i = 1; i <= n; i++) {
		if(i == source) {
			printf("%d -> %d : %d\n", source, i, dist[i]);
		}
		else {
			if(dist[i] == INF) {
				printf("%d !-> %d : %d\n", source, i, dist[i]);
				continue;
			}
			//find the path from source to i
			int path[100];
			int j = 0;
			u = i;
			while (u != source) {
				path[j++] = u;
				u = prev[u];
			}
			//print the path from source to i
			printf("%d ", source);
			for (j--; j >= 0; j--) {
				printf("-> %d ", path[j]);
			}
			printf(": %d\n", dist[i]);
		}
	}
}
int main() {
	int n, m;
	int i, j, k;
	scanf("%d %d", &n, &m);
	int adj[100][100];
	for (i = 0; i < 100; i++) {
		for (j = 0; j < 100; j++) {
			adj[i][j] = INF;
		}
	}

	int u, v, w;
	for(i = 0; i < m; ++i) {
		scanf("%d %d %d", &u, &v, &w);
		if(w < 0) {
			printf("Error: Negative edge weight\n");
			return -1;
		}
		adj[u][v] = w;
		adj[v][u] = w;
	}
	//for(int i = 1; i <= n; ++i) {
	//	for(int j = 1; j <= n; ++j) {
	//		if(adj[i][j] == INF) printf("n ");
	//		else printf("%d ", adj[i][j]);
	//	}
	//	printf("\n");
	//}

	//dijkstra(adj, n, 1);
	for(i = 1; i <= n; ++i) {
		fprintf(stderr, "%d\n", i);
		dijkstra(adj, n, i);
		fprintf(stderr, "------------------------\n");
	}
}
