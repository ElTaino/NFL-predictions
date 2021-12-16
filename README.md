# NFL-predictions
I wanted to get closer to the machine, so I decided to learn C. This is the very first project, so if anyone comes across this please feel free to make any changes. I'm always open to learning. I'm also learning x86 assembly if anyone has any advice.

I am making a program to predict NFL games. I thought it would be a fun project and some of my friends are degenerate gamblers anyways so it fits haha. 

Program info:
It starts off in Javascript / Node.js by getting season stats/info on NFL teams from the HTML elements in ESPN.com and then moving this info into files so we can use the information in C. This info includes:
- Season stats (yards per game, yards per carry, etc.)
- Schedule (home team, away team, gameID(this is a key we can append to an espn.com filepath to go to the game)).
- Game stats (yards per carry, yards per rush, etc.)
- GameId by team file (Key to link game stats)

In C:
Loading the information into several different structs that are acting (as far as I know) as a linked list of Game->Teams->stats(Offense/Defense)->Weights. Then figures out the schedule of games played for both teams in the matchup. It compares the season stats for team compared to how they performed against these teams and their averages. Those weights are used to adjust the season stats to have a more fair stat line across all teams. These stats are then used for the matchup for the home and away team to produce an expected stat line for the game. 

Further updates:
Run the weighted system for the teams in the schedule before weighting the main matchup to account for everything. Or weight the teams multiple times.
