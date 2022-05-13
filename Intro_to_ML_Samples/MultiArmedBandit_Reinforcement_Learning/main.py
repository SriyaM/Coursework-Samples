''''
Analysis of various multi-armed bandit strategies:

As seen in the graph, the mean rewards of optimistic greedy and upper confidence bound are consistenly higher than the others. The optimistic algorithm works better than the other as it does not introduce random noise to pick the best action. Instead we overestimate the rewards which allows up to explore early and maximizes returns. It also makes sense that the upper confidence bound strategy worked well as we were able to change the exploitation-exploration levels throughout the iterations which allowed us to explore early-on and then switch to more exploiting.

The softmax algorithm also yielded realtively good results as we were able to pick each arm with a probability that was proportional to its average reward.

The epsilon greedy and epsilon decay strategies were a little less effective than the strategies above. This is most likely because we depended on random chance with the greedy algorithm in which we did not optimize the exploration-exploiration ratio as time went on and with the decay strategy in which we modified the exploration-exploitation levels in a linear manner rather than basing it upon the information we already had.

As expected, the exploration strategy was the least effective as we just pulled a random lever on each turn. 

'''

import random
import matplotlib.pyplot as plt
import math
k = 10 #how many bandits there are

# each lever needs to know its expected payout (mean reward so far)
# It's been pulled n times with some rewards
# For purposes of pulling the lever, we need to know the actual payout
# (Q_n+1 = (1- 1/n)Q_n + 1/n * R_n) <-- we can just store last estimate and previous reward to calculate next predicted reward
# lever = {previousQ, previousR, n, payout}

levers = [{'previousQ':0, 'previousR':0, 'pulls':0, 'payout':random.randint(1,10)} for i in range(k)] #making lever dictionaries in a list and initializing keys

def pull(lever):
  reward = random.gauss(lever['payout'], 1) #gets random value along normal distribution based on lever payout
  if reward < 0: #prevents negative reward
    reward = 0
  lever['previousR'] = reward #records this reward
  lever['pulls'] += 1 #increases number of pulls
  lever['previousQ'] = (1 - 1/lever['pulls'])*lever['previousQ'] + lever['previousR']/lever['pulls'] #records new prediction for reward

def epsilon_greedy(n,epsilon): #takes in epsilon, number of pulls
  for lever in levers:
    lever['previousQ'] = lever['previousR'] = lever['pulls'] = 0
  total_payout = 0 #keeping track of total payout
  for i in range(n):
    explore = random.random() #random number between 0 and 1
    if explore < epsilon: #if explore 
      lever = random.choice(levers) #pull random lever
    else: #else exploit
      lever = max(levers, key=lambda x: x['previousQ']) #pull lever with max predicted payout
    pull(lever)
    total_payout += lever['previousR'] #add to total
  return total_payout #return total

def optimistic_greedy(n): #everything is pulled at least once, then you know what's the best
  for lever in levers:
    lever['previousR'] = lever['pulls'] = 0
    lever['previousQ'] = 11 #setting expected payouts too high to start
  total_payout = 0
  for i in range(n):
    lever = max(levers, key=lambda x: x['previousQ']) #pull lever with max predicted payout
    pull(lever)
    total_payout += lever['previousR'] #add to total
  return total_payout

def epsilon_decay(n, beta = 1/k): #takes in beta (speed of decay), number of pulls
  for lever in levers:
    lever['previousQ'] = lever['previousR'] = lever['pulls'] = 0
  total_payout = 0 #keeping track of total payout
  for i in range(n):
    epsilon = 1/(1+i*beta)
    explore = random.random() #random number between 0 and 1
    if explore < epsilon: #if explore 
      lever = random.choice(levers) #pull random lever
    else: #else exploit
      lever = max(levers, key=lambda x: x['previousQ']) #pull lever with max predicted payout
    pull(lever)
    total_payout += lever['previousR'] #add to total
  return total_payout #return total

def softmax(n,tau): #takes in the levers, epsilon, number of pulls
  #prob(lever l) = e**(Q_l/tau)/sum(e**(Q/tau))
  for lever in levers:
    lever['previousQ'] = lever['previousR'] = lever['pulls'] = 0
  total_payout = 0 #keeping track of total payout
  for lever in levers:
    pull(lever)
    total_payout += lever['previousR']
  for i in range(k+1,n+1):
    intervals = [math.exp(lever['previousQ']/tau) for lever in levers]
    total = sum(intervals)
    intervals = [interval/total for interval in intervals]
    for i in range(1,len(intervals)):
      intervals[i] += intervals[i-1]
    #print(intervals)
    choice = random.random() #random number between 0 and 1
    for index,upper_bound in enumerate(intervals):
      if choice < upper_bound:
        lever = levers[index]
        break
    pull(lever)
    total_payout += lever['previousR'] #add to total
  return total_payout #return total

def upper_confidence_bound(n): #takes in epsilon, number of pulls
  for lever in levers:
    lever['previousQ'] = lever['previousR'] = lever['pulls'] = 0
  payouts = [] #keeping track of total payout
  regrets = []
  for lever in levers:
    pull(lever)
    payouts.append(lever['previousR'])
    current_best = max([lever['previousQ']for lever in levers])
    regrets.append(current_best-lever['previousR'])
  for i in range(k+1,n+1):
    ucb = [(lever['previousQ']+math.sqrt(2*math.log(i)/(lever['pulls'])),lever) for lever in levers]
    lever = max(ucb, key = lambda x: x[0])[1]
    pull(lever)
    payouts.append(lever['previousR']) #add to total
    current_best = max([lever['previousQ']for lever in levers])
    regrets.append(current_best-lever['previousR'])
  return {'payouts':payouts, 'regrets':regrets} #return total

def exploration(n): #takes in epsilon, number of pulls
  for lever in levers:
    lever['previousQ'] = lever['previousR'] = lever['pulls'] = 0
  total_payout = 0 #keeping track of total payout
  for i in range(n):
    lever = random.choice(levers) #pull random lever
    pull(lever)
    total_payout += lever['previousR'] #add to total
  return total_payout #return total


# for i in range(1,51): #testing for different percents exploration
#  beta = i/50
#  print("Epsilon:",beta,"Total Reward:",epsilon_decay(beta,100))
#   for lever in levers:
#    print("Lever",levers.index(lever),": Number of Pulls:",lever['pulls'],"Actual Payout:",lever['payout'])
#  print()

# ucb = upper_confidence_bound(100)
# plt.plot(ucb['payouts'])
# plt.show()

# plt.plot(ucb['regrets'])
# plt.show()

# cumulative_payouts = []
# total = 0
# for payout in ucb['payouts']:
#   total += payout
#   cumulative_payouts.append(total)

# plt.plot(cumulative_payouts)
# plt.show()

epsilon_greedy_avg = optimistic_greedy_avg = epsilon_decay_avg = softmax_avg = ucb_avg = exploration_avg = 0

epsilon_greedy_list = []
optimistic_greedy_list = [] 
epsilon_decay_list = []
softmax_list = []
ucb_list = []
exploration_list = []
itera = []

for m in range(100):

  itera.append(m)

  ep_greed = epsilon_greedy(100,0.1)
  opti_greed = optimistic_greedy(100)
  ep_decay = epsilon_decay(100)
  softmx = softmax(100,0.86)
  ucb = sum(upper_confidence_bound(100)['payouts'])
  explr = exploration(100)

  epsilon_greedy_list.append(ep_greed)
  optimistic_greedy_list.append(opti_greed)
  epsilon_decay_list.append(ep_decay)
  softmax_list.append(softmx)
  ucb_list.append(ucb)
  exploration_list.append(explr)
  
  epsilon_greedy_avg += ep_greed/100
  optimistic_greedy_avg += opti_greed/100
  epsilon_decay_avg += ep_decay/100
  softmax_avg += softmx/100
  ucb_avg += ucb/100
  exploration_avg += explr/100

print("epsilon-greedy:",epsilon_greedy_avg)
print("optimistic-greedy:",optimistic_greedy_avg)
print("epsilon-decay:",epsilon_decay_avg)
print("softmax:",softmax_avg)
print('upper confidence bound:',ucb_avg)
print('exploration:',exploration_avg)

plt.plot(itera, epsilon_greedy_list, label = "epsilon greedy")
plt.plot(itera, optimistic_greedy_list, label = "optimistic greedy")
plt.plot(itera, epsilon_decay_list, label = "epsilon decay" )
plt.plot(itera, softmax_list,label = "softmax" )
plt.plot(itera,ucb_list, label = "upper confidence bound")
plt.plot(itera, exploration_list, label = "exploration")
plt.xlabel('Iteration')
plt.ylabel('Mean reward over time')
plt.title('Multi Armed Bandit Rewards ')
plt.legend()
plt.show()

# total reward over time
# mean reward over time
# regret at each step (difference between reward and best Q)
# total regret