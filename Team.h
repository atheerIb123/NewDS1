#ifndef TEAM_H_
#define TEAM_H_

#include "AVLTree.h"
#include "PlayerByStats.h"
#include "PlayerById.h"

class Team
{
private:	
	AVLTree<PlayerByStats> teamTreeByStats;
	AVLTree<PlayerById> teamTreeById;
	std::shared_ptr<int> teamId;
	int points;
	int goalKeepers[2];
	int totalGoals;
	int totalCards;
	int totalPlayers;
	int topScorer[3];
	std::shared_ptr<int> totalGamesPlayed;
public:
	Team();
	Team(int teamId, int points);
	~Team() = default;
	Team(const Team& other) = default;
	Team& operator=(const Team& other) = default;

	bool insertPlayer(int playerId, int gamesPlayed, int goals, int cards, bool goalKeeper);
	bool removePlayer(int playerId);
	int getTeamPoints() const;
	void addPoints(int points);
	int getTopScorer() const;
	int getTopScorerGoalsAmount() const;
	int getTeamPower() const;
	int getPlayersCount() const;
	int getID() const;
	void setNewId(int id);
	void getPlayersDetails(PlayerByStats* const out);
	bool hasGoalKeeper() const;
	void updatePlayerStatsInTeam(PlayerByStats& p, int playerId, int gamesToAdd, int goalsToAdd, int cardsToAdd);
	void inOrderPlayers(int* sortedPlayers, PlayerByStats* const output) const;
	void inOrderPlayers(PlayerById* const output) const;
	Node<PlayerById>* getRootOfIds() const;
	void mergeTeams(Team& other);
	void incrementGamesPlayed();
	void setGamesPlayed();
	Node<PlayerByStats>* findPlayerById(int playerId);
    std::shared_ptr<int>& teamIdPtr();
	bool operator<(const Team& other) const;
	bool operator>(const Team& other) const;
	bool operator==(const Team& other) const;
};

#endif