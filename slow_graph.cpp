//Ruslan Gatiatuintin
#include <bits/stdc++.h>
#include <utility>
using namespace std;
typedef long long ll;

//Implementation of priority queue is borrowed from https://github.com/deangelacgn/generic-priority-queue/tree/master


template < typename ValueType, typename Compare = std::less<ValueType>() >
class PQ
{
public:
    //=== Aliases
    typedef size_t size_type;
    typedef ValueType value_type;
    typedef const value_type& const_reference;

private:
    //=== Members
    size_type m_capacity;
    size_type m_length;
    bool m_sorted;
    std::unique_ptr< ValueType [] > m_data;
    Compare m_cmp;

    enum { DEFAULT_SIZE = 300001 };

    void fix_heap( size_type item_idx );

    void move_up( size_type item_idx );

    void reserve( size_type new_cap );

    bool full( void ) const { return m_length == m_capacity; }

public:
    //=== The public interface.

    //=== Constructors and destructor.

    explicit PQ( const Compare& cmp = Compare() )
        :m_cmp(cmp),
         m_capacity(DEFAULT_SIZE),
         m_sorted(true),
         m_length(0)
    {
        m_data = std::make_unique< ValueType [] >(m_capacity);
    };


    PQ( const std::initializer_list< ValueType >& ilist, const Compare& cmp = Compare() );

    template< typename InputIt >
    PQ( InputIt first, InputIt last, const Compare& cmp = Compare() );
    PQ( const PQ & other);


    PQ & operator= ( const PQ& rhs );

    virtual ~PQ() {    };

    //=== Element access methods
    const_reference  top( void );
    //=== Capacity methods

    bool empty( void ) const { return m_length==0; }


    size_type size( void ) const { return m_length; }

    //=== Modifier methods.

    void clear();

    void push( const_reference x );

    void pop();

};
template < typename ValueType, typename Compare >
void PQ< ValueType, Compare >::fix_heap( size_type item_idx )
{
    size_type top_value = item_idx;

    if(((2 * item_idx + 1) < m_length) && m_cmp(m_data[2 * item_idx + 1], m_data[top_value]))
    {
        top_value = 2 * item_idx + 1;
    }

    if(((2 * item_idx + 2) < m_length) && m_cmp(m_data[2 * item_idx + 2], m_data[top_value]))
    {
        top_value = 2 * item_idx + 2;
    }

    if(top_value != item_idx)
    {
        //faz o swap das posicoes
        std::swap(m_data[item_idx], m_data[top_value]);

        fix_heap(top_value);
    }

}


template < typename ValueType, typename Compare >
void PQ< ValueType, Compare >::move_up( size_type item_idx )
{
    // enquanto o pai for maior ou menor do que o filho
    while(m_cmp(m_data[item_idx], m_data[(item_idx - 1)/2]) && item_idx > 0)
    {
        // swap entre o pai e o filho
        std::swap(m_data[(item_idx - 1)/2], m_data[item_idx]);

        item_idx = (item_idx - 1)/2;
    }
}


template < typename ValueType, typename Compare >
void PQ< ValueType, Compare >::reserve( size_type new_cap )
{

    ValueType * temp =  new ValueType[new_cap];

    for(size_t i = 0; i < new_cap; i++)
    {
        temp[i] = m_data[i];
    }

    m_data.reset( temp );

    m_capacity = new_cap;
}


template < typename ValueType, typename Compare >
void PQ< ValueType, Compare >::push( const_reference x )
{
    //verifica se o array suporta a insercao
    if(m_length >= m_capacity)
    {
        reserve(2 * m_capacity);
    }

    if(m_sorted == false)
    {
        for(auto i = m_length/2; i > 0; i--)
        {
            fix_heap(i-1);
        }

        m_sorted = true;
    }

    m_data[m_length] = x;

    if(m_cmp(m_data[m_length], m_data[(m_length - 1)/2]) && m_length > 0 )
    {
        move_up(m_length);
    }

    m_length++;
}


template < typename ValueType, typename Compare >
void PQ< ValueType, Compare >::pop()
{
    //Reorganiza a heap antes de fazer o pop
    if(m_sorted == false)
    {
        for(auto i = m_length/2; i > 0; i--)
        {
            fix_heap(i-1);
        }

        m_sorted = true;
    }

    std::swap(m_data[0], m_data[m_length-1]);

    m_length --;

    //Move o elemento que tá na raiz pra baixo, reorganizando a heap(substitui o move_down)
    fix_heap(0);

}


template < typename ValueType, typename Compare >
typename PQ< ValueType, Compare >::const_reference PQ< ValueType, Compare >::top( void )
{
    //Reorganiza a heeap antes de liberar o topo
    if(m_sorted == false)
    {
        for(auto i = m_length/2; i > 0; i--)
        {
            fix_heap(i-1);
        }

        m_sorted = true;
    }

    return m_data[0];
}

template < typename ValueType, typename Compare >
template < typename InputIt >
PQ< ValueType, Compare >::PQ( InputIt first, InputIt last, const Compare & cmp ): m_cmp(cmp)
{
    m_capacity = DEFAULT_SIZE;
    m_sorted = true;
    m_length = 0;
    m_data = std::make_unique< ValueType [] >(m_capacity);

    for(auto it = first; it != last; it++)
    {
        (*this).push(*it);
    }
}

template < typename ValueType, typename Compare >
PQ< ValueType, Compare >::PQ( const std::initializer_list< ValueType > & ilist , const Compare & cmp ):m_cmp(cmp)
{
    m_capacity = DEFAULT_SIZE;
    m_sorted = true;
    m_length = 0;
    m_data = std::make_unique< ValueType [] >(m_capacity);

    for(const auto it: ilist)
    {
        (*this).push(it);
    }
}

template < typename ValueType, typename Compare >
PQ< ValueType, Compare >::PQ( const PQ & other):m_cmp(other.m_cmp)
{
    *this = other;
}

template < typename ValueType, typename Compare >
PQ< ValueType, Compare > & PQ< ValueType, Compare >::operator=( const PQ< ValueType, Compare > & rhs )
{

    m_capacity = rhs.m_capacity;
    m_sorted = rhs.m_sorted;
    m_length = rhs.m_length;
    m_data = std::make_unique< ValueType [] >(m_capacity);

    for(size_t i = 0; i < m_capacity; i++)
    {
        m_data[i] = rhs.m_data[i];
    }


    return *this;

}

auto compare = []( pair<pair<ll,ll>,ll> a, pair<pair<ll,ll>,ll> b ) { return a > b; };

template <typename V,typename E, typename T = int>
class Graph {
public:
    list<list<int>> adjacencyList;
    list<pair<E, pair<int, int>>> edgeList;
    vector<vector<T>> adjMatrix;
    vector<V> vertices;
    list<V> vertexList;
    vector<bool> visited;

    size_t numOfVertices;
    size_t numOfEdges;
};

template <typename V, typename E, typename T = int>
class DirectedWeightedGraph: Graph<V,E,T> {
    vector<int> representative, sizes;
    vector<vector<pair<int, E>>> treeAdj;

    int find(int a) {
        if (a != representative[a])
            representative[a] = find(representative[a]);
        return representative[a];
    }

    void unite(int a, int b) {
        a = find(a);
        b = find(b);
        if (sizes[a] < sizes[b])
            swap(a, b);
        representative[b] = a;
        sizes[a] += sizes[b];
    }

public:
    DirectedWeightedGraph(vector<pair<E, pair<int, int>>> edges, vector<V> vertices, vector<vector<int>> adj) {
        //quick_sort(edges.begin(), edges.end());

        this->numOfVertices = vertices.size();
        this->numOfEdges = edges.size();
        this->vertices = vertices;

        for (const auto& v: vertices)
            this->vertexList.push_back(v);
        for (const auto& e: edges)
            this->edgeList.push_back(e);


        for (auto & i : adj) {
            this->adjacencyList.emplace_back();
            for (int & j : i) {
                this->adjacencyList.back().emplace_back(j);
                this->numOfEdges++;
            }
        }
    }

    //I know that ideally I should have a user function for constrain but too complicated for this exercise
    DirectedWeightedGraph(vector<vector<T>> adjMatrix, T constrain, vector<V> vertices) {
        this->numOfVertices = adjMatrix.size();
        this->adjMatrix.resize(this->numOfVertices, vector<pair<ll,ll>>(this->numOfVertices));
        this->vertices = vertices;

        for (int i = 0; i < this->numOfVertices; i++)
            for (int j = 0; j < this->numOfVertices; j++)
                if (adjMatrix[i][j].second >= constrain.second)
                    this->adjMatrix[i][j] = adjMatrix[i][j];
    }


    //Dijkstra algorithm
    void RuslanGatiatullin_sp(ll source, ll finish) {
        vector<pair<ll, ll>> distances(this->numOfVertices, {INT32_MAX, INT32_MAX});
        PQ<pair<pair<ll,ll>,ll>, decltype(compare)> q(compare);
        vector<bool> processed(this->numOfVertices);
        vector<ll> from(this->numOfVertices, -1);
        from[source] = source;
        distances[source] = {0, INT32_MAX};
        q.push({make_pair(0, INT32_MAX), source});

        while(!q.empty()) {
            ll a = q.top().second;
            q.pop();
            if (processed[a]) continue;
            processed[a] = true;

            for (int i = 0; i < this->numOfVertices; i++) {
                ll b = i, w = this->adjMatrix[a][i].first;
                if (make_pair(0ll, 0ll) != this->adjMatrix[a][i] && distances[a].first + w < distances[b].first) {
                    distances[b].first = distances[a].first + w;
                    distances[b].second = min(this->adjMatrix[a][i].second, distances[a].second);
                    from[b] = a;
                    q.push({make_pair(-distances[b].first, distances[b].second), b});
                }
            }
        }
        if (distances[finish].first == INT32_MAX) { cout << "IMPOSSIBLE\n"; return; }
        list<V> path;
        ll node = finish;
        while (node != source) {
            path.push_front(this->vertices[node]);
            node = from[node];
        }
        path.push_front(this->vertices[source]);

        cout << path.size() << " " << distances[finish].first << " " << distances[finish].second << "\n";
        for (const auto& v: path)
            cout << v << " ";
    }

    //A standard Kruskal's algorithm, since it by default works on disconnected graphs
    auto RuslanGatiatuintin_mst() {

        //sorting is done in the constructor as it's much easier, time complexity: O(mlogm) where m is number of edges

        representative.clear(), sizes.clear(), treeAdj.clear();
        representative.resize(this->numOfVertices);
        sizes.resize(this->numOfVertices);
        treeAdj.resize(this->numOfVertices);

        for (int i = 0; i < this->numOfVertices; i++) //time complexity: O(n) where n is number of vertices
            representative[i] = i, sizes[i] = 1;

        for (auto smallestEdge: this->edgeList) { //time complexity: O(mlogn)
            int from = smallestEdge.second.second, to = smallestEdge.second.first;
            auto w = smallestEdge.first;
            if (find(to) != find(from)) {
                unite(from, to);
                treeAdj[from].emplace_back(to, w); //adding edges to a tree
                treeAdj[to].emplace_back(from, w);
            }
        }
        return treeAdj;
    }

    //Bellman-Ford algorithm
    list<V> RuslanGatiatullin_sp() {
        vector<int> distances(this->numOfVertices, 2000000000);
        vector<int> pred(this->numOfVertices);
        distances[0] = 0;
        bool improvement = false;

        for (int i = 0; i < this->numOfVertices - 1; i ++) {
            improvement = false;
            for (auto e: this->edgeList) {
                int w = e.first, from = e.second.first, to = e.second.second;
                if (distances[to] > w + distances[from]) {
                    distances[to] = w + distances[from];
                    pred[to] = from;
                    improvement = true;
                }
            }
            if (!improvement) break;
        }
        improvement = false;
        int cycleEnd;
        for (auto e: this->edgeList) {
            int w = e.first, from = e.second.first, to = e.second.second;
            if (distances[to] > w + distances[from]) {
                cycleEnd = from;
                improvement = true;
                break;
            }
        }
        if (!improvement) return {};

        list<V> negCycle;
        this->visited.clear(), this->visited.resize(this->numOfVertices);
        while(!this->visited[cycleEnd]) {
            this->visited[cycleEnd] = true;
            negCycle.push_front(this->vertices[cycleEnd]);
            cycleEnd = pred[cycleEnd];
        }
        while(negCycle.back() != this->vertices[cycleEnd]) negCycle.pop_back();
        return negCycle;
    }

    int traverseTree(int noww, int fromm) {
        int vertInComponent = 0;
        vector<pair<int,int>> st;
        st.emplace_back(noww, fromm);
        while (!st.empty()) {
            int now = st.back().first, from = st.back().second;
            st.pop_back();
            this->visited[now] = true;
            vertInComponent++;

            for (auto u: this->treeAdj[now])
                if (u.first != from) st.push_back(make_pair(u.first, now));
        }
        return vertInComponent;
    }

    void printTreeComponent(int beginning, int vertInComponent) {
        cout << vertInComponent << " " << this->vertices[beginning] << "\n";

        vector<pair<int,int>> st;
        st.emplace_back(beginning, -1);
        while (!st.empty()) {
            int now = st.back().first, from = st.back().second;
            st.pop_back();
            for (const auto& u: this->treeAdj[now]) {
                if (u.first != from) {
                    cout << this->vertices[now] << " " << this->vertices[u.first]
                         << " " << u.second << "\n";
                    st.push_back(make_pair(u.first, now));
                }
            }
        }
    }

    vector<pair<V,int>> findTreeComponents() {
        vector<pair<V,int>> a;
        this->visited.resize(this->numOfVertices, 0);
        for (int i = 0; i < this->numOfVertices; i++) {
            if (!this->visited[i]) {
                this->visited[i] = true;
                a.emplace_back(i, traverseTree(i, -1));
            }
        }
        return a;
    }
};

int main() {
    ios::sync_with_stdio(false); cin.tie(); cout.tie(); //fast input and output
    ll n, m; cin >> n >> m;
    vector<pair<int, pair<int, int>>> edgeList;
    vector<ll> vert(n);
    vector<vector<int>> adj;
    vector<vector<pair<ll,ll>>> adjMatrix(n, vector<pair<ll,ll>>(n));

    for (int i = 0; i < n; i++) vert[i] = i + 1;
    for (ll i = 0, from, to, len, bw; i < m; i++) {
        cin >> from >> to >> len >> bw;
        from--, to--;
        adjMatrix[from][to] = {len, bw};
    }
    ll source, finish, minBW; cin >> source >> finish >> minBW;
    source--, finish--;
    DirectedWeightedGraph<ll,ll, pair<ll,ll>> graph(adjMatrix, make_pair(0, minBW), vert);
    graph.RuslanGatiatullin_sp(source, finish);


    return 0;
}
