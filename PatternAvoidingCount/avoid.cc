/*********************************************
 * Enumerating Pattern Avoiding Permutations *
 * (C) Yuma INOUE (June. 11, 2022)           *
 * All rights reserved.                      *
 *********************************************/

#include<iostream>
#include<cstdio>
#include<cstdlib>
#include<cstring>
#include<ctime>
#include<vector>
#include<utility>
#include<algorithm>
#include<cctype>
#include<set>
#include<queue>
#include<sys/time.h>
#include<sys/resource.h>
#include"RotPiDD.h"
#include"ZBDD.h"
#include"BDD.h"
#include"CtoI.h"
using namespace std;

typedef string Int;

//Permutation Pattern Class
struct PermPattern{
  int length;
  vector<int> permutation;
  vector<int> adjacent;
  vector<int> consecutive;

  PermPattern(string s = ""){
    permutation.clear(); adjacent.clear(); consecutive.clear();

    //insert blanks to parse
    if(isdigit(s[0]))s = " " + s;
    if(isdigit(s[(int)s.size()-1]))s = s + " ";

    //parse string to pemutation pattern
    for(int i=0;i<s.size();i++){
      int x = 0;
      while(isdigit(s[i]))x *= 10, x+=s[i]-'0', i++;
      if(x)permutation.push_back(x);

      if(i==s.size())break;

      if(s[i] == ' ' || s[i] == '-')adjacent.push_back(0);
      else adjacent.push_back(1);

      if(s[i] == ' ' || s[i] == '_')consecutive.push_back(0);
      else consecutive.push_back(1);
    }

    length = permutation.size();
  }

  PermPattern(vector<int> a,vector<int> b,vector<int> c)
    :permutation(a),adjacent(b),consecutive(c){
    length = a.size();
  }
};

//for counting cardinality
unordered_map<bddword,Int> card_memo;

inline Int add(Int a, Int b){
  if(a.size() < b.size())return add(b,a);

  int carry = 0;
  for(int i=0;i<(int)b.size();i++){
    int val = a[i]-'0' + b[i]-'0' + carry;
    if(val>9){
      val -= 10; carry = 1;
    }else carry = 0;
    a[i] = val+'0';
  }

  for(int i=b.size();carry && i<(int)a.size();i++){
    int val = a[i]-'0' + carry;
    if(val>9){
      val -= 10; carry = 1;
    }else carry = 0;
    a[i] = val+'0';
  }
  if(carry)a += "1";

  return a;
}

Int cardinality_internal(const ZBDD &z){
  if(z==0)return "0";
  if(z==1)return "1";
  bddword id = z.GetID();
  if(card_memo.find(id)!=card_memo.end())return card_memo[id];

  Int res = add( cardinality_internal(z.OffSet(z.Top())), cardinality_internal(z.OnSet0(z.Top())) );
  return card_memo[id] = res;
}

Int cardinality(const ZBDD &z){
  Int cardinality_ = cardinality_internal(z);
  reverse(cardinality_.begin(), cardinality_.end());
  return cardinality_;
}

//only [a,b] represent all permutation and other elements are fixed (1-origin)
RotPiDD Partial_Shuffle(int a, int b){
  RotPiDD res(1);
  for(int i=a;i<=b;i++){
    for(int j=i+1;j<=b;j++){
      res += res.LeftRot(i,j);
    }
  }
  return res;
}

//if adj[i] = 1, i-th and (i+1)-th element must be adjacent
RotPiDD Enum_Pos(int n, int k, vector<int> adj){
  RotPiDD dp[n+1][k+1];
  for(int i=0;i<=n;i++)
    for(int j=0;j<=k;j++)dp[i][j] = RotPiDD(0);
  dp[0][0] = RotPiDD(1);

  for(int j=0;j<=k;j++){
    for(int i=j;i<=n;i++){
      if(i!=n){
	if(j!=k)dp[i+1][j+1] += dp[i][j].LeftRot(j+1,i+1);
	if(!adj[j])dp[i+1][j] += dp[i][j];
      }
    }
  }
  return dp[n][k];
}

//if cons[i] = 1, i-th and (i+1)-th element must be consecutive
RotPiDD Enum_Comb(int n, int k, vector<int> cons){
  RotPiDD dp[n+1][k+1];
  for(int i=0;i<=n;i++)
    for(int j=0;j<=k;j++)dp[i][j] = RotPiDD(0);
  dp[k][k] = RotPiDD(1);

  for(int i=k;i<=n;i++){
    for(int j=k;j>=0;j--){
      if(i!=n && !cons[k-j])dp[i+1][j] += dp[i][j].LeftRot(i+1-j,i+1);
      if(j!=0)dp[i][j-1] += dp[i][j];
    }
  }
  return dp[n][0];
}

RotPiDD Enum_CombBase(int n, int k){
  RotPiDD I[n+1]; I[k] = 1;
  for(int i=k+1;i<=n;i++){
    I[i] = I[i-1];
    for(int j=1;j<i;j++){
      I[i] = I[i] + I[i-1].LeftRot(j,i);
    }
  }
  return I[n];
}

//output the set of n-element permutation which include "pattern"
RotPiDD Pattern_Hit(int n, PermPattern pattern){
  int k = pattern.length;

  RotPiDD A = Enum_CombBase(n,k);
  RotPiDD B = RotPiDD::VECtoRotPiDD(pattern.permutation);
  RotPiDD C = Enum_Pos(n,k,pattern.adjacent);

  return C * B * A;
}

RotPiDD Pattern_Avoid(int n, PermPattern pattern){
  RotPiDD univ = Partial_Shuffle(1,n);
  if(n<pattern.length)return univ;
  return univ - Pattern_Hit(n,pattern);
}

int main(int argc,char* argv[]){
  if(argc < 2)return 0;

  int N = atoi(argv[1]);

  BDD_Init(10000LL,1500000000LL);
  for(int i=0;i<N;i++)RotPiDD_NewVar();

  if(argc == 2){
    string pattern;
    getline(cin,pattern);

    RotPiDD Av = Pattern_Avoid(N,PermPattern(pattern));
    cout << "Cardinality: " << cardinality(Av.GetZBDD()) << endl;
  }else{
    int len = strlen(argv[2]);
    string s;
    for(int i=0;i<len;i++){
      if(argv[2][i] == '+')s += " ";
      else s += argv[2][i];
    }
    PermPattern p(s);

    RotPiDD Av = Pattern_Avoid(N,p);
    cout << "Cardinality: " << cardinality(Av.GetZBDD()) << endl;
   }

  struct rusage Use;
  getrusage(RUSAGE_SELF,&Use);
  printf("Usr Time: %ld.%06ld sec\n",Use.ru_utime.tv_sec,Use.ru_utime.tv_usec);
  printf("Sys Time: %ld.%06ld sec\n",Use.ru_stime.tv_sec,Use.ru_stime.tv_usec);
  printf("Use Mems: %ld KB\n",Use.ru_maxrss);
}
