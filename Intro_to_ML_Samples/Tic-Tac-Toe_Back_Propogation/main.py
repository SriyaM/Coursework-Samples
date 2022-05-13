#Tic-Tac-Toe with Back-propogation

# learning_rate = 1/count(state)
import random, math
from collections import defaultdict
from matplotlib import pyplot as plt

DISCOUNT = 0.9
PENALTY = -1 #penalty for taking another turn

#back-propogating values
WIN = 30
TIE = 0
LOSS = -10

ITERATIONS = 1

win_states = (
  [1]*3 + [0]*3 + [0]*3,
  [0]*3 + [1]*3 + [0]*3,
  [0]*3 + [0]*3 + [1]*3,
  [1,0,0]*3,
  [0,1,0]*3,
  [0,0,1]*3,
  [1,0,0] + [0,1,0] + [0,0,1],
  [0,0,1] + [0,1,0] + [1,0,1]
) #eight possible ways to win (row, column, diagonal)

WIN_STATES = tuple(frozenset(i for i,e in enumerate(state) if e == 1) for state in win_states) #creating a frozen set of frozen sets for indexes of ones in each win state

rewards = defaultdict(int) #defaults to zero
counts = defaultdict(int) #keeps track of number of times we've been to that state

#going to use upper confidence bound
def turn(player, board):
  zeros = [i for i,e in enumerate(board) if e == 0]
  max_reward = LOSS - 1
  for space in zeros:
    new_state = board[:]
    new_state[space] = player
    reward = rewards[tuple(new_state)] + math.sqrt(2 * ITERATIONS / (counts[tuple(new_state)] + 1))
    if reward > max_reward:
      best_space = space
      max_reward = reward
  board[best_space] = player

def won(player,board):
  pieces = frozenset(i for i,e in enumerate(board) if e == player) #getting indeces of where the player's pieces currently are
  for state in WIN_STATES:
    if state.issubset(pieces):
      return(True)
  return(False)

def update_rewards(states,reward): # taking in reversed states
  # change to use optimal future reward among all the actions at state t+1
  # get state after opponent took their turn
  # get reward values from all possible actions from that states
  # update using the best reward value
  # (not back-propogating actual reward)
  player = 1 if sum(states[0]) else -1
  rewards[states[0]] = reward #last move gets full reward
  counts[states[0]] += 1
  for i,state in enumerate(states[2::2]):
    #opponent's next move at i-1
    #our actual next move at i-2
    zeros = [i for i,e in enumerate(states[i-1]) if e == 0]
    max_reward = rewards[states[i-2]]
    for space in zeros:
      new_state = list(states[i-1][:])
      new_state[space] = player
      reward = rewards[tuple(new_state)]
      if reward > max_reward:
        max_reward = reward
    counts[state] += 1 #increasing count of number of times at this state
    rewards[state] += (PENALTY + DISCOUNT*max_reward - rewards[state])/counts[state] #new Q value
    reward = rewards[state]

def play_well(player,board): #using knowledge to play well
  max_reward = LOSS-1
  zeros = [i for i,e in enumerate(board) if e == 0]
  for space in zeros:
    new_state = board[:]
    new_state[space] = player
    if counts[tuple(new_state)] > 0:
        reward = rewards[tuple(new_state)] + math.sqrt(2 * ITERATIONS / (counts[tuple(new_state)]))
    else:
        reward = float('inf')
    if reward > max_reward:
      best_space = space
      max_reward = reward
  board[best_space] = player

def play_game():
  board = [0]*9
  states = []
  player = 1
  tied = True
  global ITERATIONS
  ITERATIONS += 1
  for t in range(9): #9 possible turns
    #have player take their turn
    turn(player,board)
    states.append(tuple(board)) #allows us to back-propogate
    #check if they won
    if won(player,board): #if they won
      tied = False #they didn't tie
      # print(player, board) #printing the board if someone won
      break
    #switch player
    player *= -1 #switch player
  states.reverse()
  if tied:
    update_rewards(states, TIE) #player 1's moves, back-propgating tie reward
    update_rewards(states[1:], TIE) #player -1's moves, back-propagating tie reward
  else:
    update_rewards(states, WIN) #player who won will be most recent move
    update_rewards(states[1:], LOSS)
  return player if not tied else 0

def play_against_computer():
  board = [0]*9
  player = 1 #computer
  for turn in range(9):
    if player == 1:
      play_well(player,board)
    else:
      for row in range(3):
        print(board[row*3:row*3+3]) #printing the board
      space = int(input("Enter a space number:"))
      board[space] = player
    if won(player,board):
        break
    player *= -1
  for row in range(3):
    print(board[row*3:row*3+3]) #printing the board

cumulative_player1 = [0]
cumulative_player2 = [0]
cumulative_ties = [0]

for i in range(10000):
  winner = play_game()
  cumulative_player1.append(cumulative_player1[-1] + (1 if winner == 1 else 0))
  cumulative_player2.append(cumulative_player2[-1] + (1 if winner == -1 else 0))
  cumulative_ties.append(cumulative_ties[-1] + (1 if winner == 0 else 0))

plt.plot(cumulative_player1, label = "player 1")
plt.plot(cumulative_player2)
plt.plot(cumulative_ties)

# Plot for assignment
plt.plot([i + j for i, j in zip(cumulative_ties, cumulative_player2)])
plt.legend()
plt.show()
