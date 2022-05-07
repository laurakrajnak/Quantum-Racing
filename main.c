#include <stdio.h>

#define R_MAX 2147483647 // all generated numbers are less than R_MAX
static long long unsigned int SEED = 0x1;
void srnd(int seed) { SEED = seed; }

int rnd(int from, int to) {
    SEED = SEED * 16807 % R_MAX;
    return from + (int) SEED % (to - from + 1);
}

void exit(int status);

int control_stn(int s, int t, int n) {  // input control
    if (s <= 0 | n < 10 | n > 100 | 2 * t > n / 2) {
        exit(1);
    }
}

int control_t(int arrT[][2], int t, int n) {  // input control of tunnels
    for (int i=0;i<t;i++) {
        for (int j=0;j<2;j++) {
            if (arrT[i][j] <= 0 | arrT[i][j] >= n - 1) { // start/end/off track
                exit(2);
            }
        }
        for (int m=0;m<t;m++) {
            if ( i != m & arrT[i][0] == arrT[m][0]) { // different entrances
                exit(2);
            } else if (arrT[i][0] == arrT[m][1]) { // exit is not an entrance
                exit(2);
            }
        }
    }
}

void sort_t(int arrT[][2], int t) {
    printf("TUNNELS:");
    int higher[2];
    int i, j, k;

    for (j=0;j<t-1;j++) {  // tunnel arrangement
        for (i=0;i<t-1;i++) {
            if (arrT[i][0] > arrT[i+1][0]) {
                higher[0] = arrT[i][0];
                higher[1] = arrT[i][1];
                arrT[i][0] = arrT[i+1][0];
                arrT[i][1] = arrT[i+1][1];
                arrT[i+1][0] = higher[0];
                arrT[i+1][1] = higher[1];
            }
        }
    }
    for (k=0;k<t;k++) {  // listing of tunnels in the correct order
        printf(" [%d,%d]", arrT[k][0], arrT[k][1]);
    }
}

int game_entry() {  // player before the start
    int r1 = rnd(1,6), r2 = rnd(1,6);
    printf("[%d,%d] ", r1, r2);

    if (r1 + r2 > 7) {
        return r1 + r2 - 7;
    } else {
        return -1;
    }
}

int bigger_r(int r1, int r2) {  // bigger dice
    if (r1 > r2) {
        return r1;
    } else {
        return r2;
    }
}


void game(int n, int t, int arrT[][2]) {
    int round = 1, player_num = 1;
    int pos_after1 = -1, pos_after2 = -1, pos_before, pos_after, old_after;
    int r1, r2, a, b;
    int visits[n];
    for (a=0;a<n;a++) {
        visits[a] = 0;
    }

    while (pos_after1 < n & pos_after2 < n) {  // START
        int teleport = 0, special_case = 0, elimination = 0;

        if (player_num == 1) {  // who's turn is it?
            pos_after = pos_after1;
        } else {
            pos_after = pos_after2;
        }
        printf("\n[%d,%d] ", round, player_num);

        pos_before = pos_after;
        if (pos_after < 0) {  // player before the start
            printf("[%d] ", pos_before);
            pos_after = game_entry();
        } else {  // player on the track
            r1 = rnd(1, 6), r2 = rnd(1, 6);
            pos_after = pos_before + bigger_r(r1, r2);
        }

        for (int i = 0; i < t + 1; i++) {  // teleport
            if (pos_after == arrT[i][0]) {
                pos_after = arrT[i][1];
                teleport = 1;
            }
        }
        if (old_after > 0 & pos_after > 0) {
            // special cases
            if (((pos_before < old_after) & (r1 + r2 == 12)) | ((pos_before > old_after) & (r1 + r2 == 2))) {
                pos_after = old_after;
                if (player_num == 1) {
                    pos_after2 = pos_before;
                } else {
                    pos_after1 = pos_before;
                }
                visits[pos_before]++;
                special_case = 1;
            }
            if (pos_after == old_after & special_case == 0) {  // elimination
                if (player_num == 1) {
                    pos_after2 = -1;
                } else {
                    pos_after1 = -1;
                }
                elimination = 1;
            }
        }

        if (pos_before > 0) {  // listing of rounds
            printf("[%d] [%d,%d] [%d]", pos_before, r1, r2, pos_after);
        } else {
            printf("[%d]", pos_after);
        }

        if (special_case == 1) {  // listing S T E
            elimination = 0;
            teleport = 0;
            printf(" S");
        }
        if (teleport == 1) {
            printf(" T");
        }
        if (elimination == 1) {
            printf(" E");
        }
        if (pos_after > 0 & pos_after < n) {  // listing VISITS
            visits[pos_after]++;
        }

        old_after = pos_after;  // save values for next round ("previous player")
        if (player_num == 1) {  // assign a general value back to a specific player
            pos_after1 = pos_after;
        } else {
            pos_after2 = pos_after;
        }
        player_num = 1 + (player_num % 2);  // next player
        round++;  // next round
    }
    player_num = 1 + (player_num % 2);
    printf("\nWINNER: %d", player_num);
    printf("\nVISITS:");
    for (b=0;b<n;b++) {
        printf(" %d", visits[b]);
    }
}


int main() {
    int s, t, n;  // s - initialization value for number generator, t - number of tunnels, n - path length
    scanf("%d %d %d", &s, &t, &n);
    control_stn(s, t, n);
    srnd(s);

    int arrT[t][2], i;  // tunnels
    for (i=0;i<t;i++) {
        scanf("%d %d", &arrT[i][0], &arrT[i][1]);
    }
    control_t(arrT, t, n);
    sort_t(arrT, t);

    game(n, t, arrT);
    return 0;
}