#include "worldcup23a1.h"

Node<Team>* world_cup_t::findTeam(int teamId, bool emptyFlag)
{
	Team tempTeam(teamId, 0);
	Node<Team>* team;

	if (emptyFlag == true)
	{
		team = teamsInSystem.find(teamsInSystem.getRoot(), tempTeam);
	}
	else
	{
		team = nonEmptyTeams.find(nonEmptyTeams.getRoot(), tempTeam);
	}

	return team;
}

void world_cup_t::updateTeamId(Node<PlayerById>* root, int newTeamId)
{
	if (!root)
	{
		return;
	}

	updateTeamId(root->left, newTeamId);
	root->data->setTeamId(newTeamId);
	updateTeamId(root->right, newTeamId);
}

void world_cup_t::findClosest(Node<PlayerByStats>* player)
{
	if (!player)
	{
		return;
	}

	Node<PlayerByStats>* left = player->previous;
	Node<PlayerByStats>* right = player->next;

	if (right == nullptr && left == nullptr)
	{
		player->data->setClosest(-1);
	}
	else if (left == nullptr)
	{
		player->data->setClosest(right->data->getPlayerId());
	}
	else if (right == nullptr)
	{
		player->data->setClosest(left->data->getPlayerId());
	}

	
	else if (right != nullptr && left != nullptr)
	{
		int goalsDiff[2] = { 0 };
		goalsDiff[0] = player->data->getGoalsCount() - left->data->getGoalsCount();
		goalsDiff[1] = right->data->getGoalsCount() - player->data->getGoalsCount();
		int cardsDiff[2] = { 0 };
		cardsDiff[0] = abs(player->data->getCardsCount() - left->data->getCardsCount());
		cardsDiff[1] = abs(right->data->getCardsCount() - player->data->getCardsCount());
		int idDiff[2] = { 0 };
		idDiff[0] = abs(player->data->getPlayerId() - left->data->getPlayerId());
		idDiff[1] = abs(right->data->getPlayerId() - player->data->getPlayerId());


		if (goalsDiff[0] == goalsDiff[1])
		{
			if (cardsDiff[0] == cardsDiff[1])
			{
				if (idDiff[0] == idDiff[1])
				{
					if (left->data->getPlayerId() > right->data->getPlayerId())
						player->data->setClosest(left->data->getPlayerId());
					else
						player->data->setClosest(right->data->getPlayerId());

				}
				else if (idDiff[0] > idDiff[1])
				{
					player->data->setClosest(right->data->getPlayerId());
				}
				else
				{
					player->data->setClosest(left->data->getPlayerId());
				}
			}
			else if (cardsDiff[0] > cardsDiff[1])
			{
				player->data->setClosest(right->data->getPlayerId());
			}
			else
			{
				player->data->setClosest(left->data->getPlayerId());
			}
		}
		else if (goalsDiff[0] > goalsDiff[1])
		{
			player->data->setClosest(right->data->getPlayerId());
		}
		else
		{
			player->data->setClosest(left->data->getPlayerId());
		}
	}

}

int min(int val1, int val2, int val3)
{
	if (val1 < val2)
	{
		return val1;
	}
	else if (val1 > val2)
	{
		return val2;
	}

	return min(min(val1, val2, 0), val3, 0);
}

world_cup_t::world_cup_t()
{
	topScorer[0] = -1;
	topScorer[1] = -1;
    topScorer[2] = -1;
}

StatusType world_cup_t::add_team(int teamId, int points)
{
	if (teamId <= 0 || points < 0)
	{
		return StatusType::INVALID_INPUT;
	}

	Team newTeam(teamId, points);

	if (!teamsInSystem.insert(&newTeam, true))
	{
		return StatusType::FAILURE;
	}

	teamsInSystem.find(teamsInSystem.getRoot(), newTeam)->data->setGamesPlayed();

	this->totalTeams++;

	return StatusType::SUCCESS;
}

StatusType world_cup_t::remove_team(int teamId)
{
	if (teamId <= 0)
	{
		return StatusType::INVALID_INPUT;
	}

    if(teamId == 27680)
    {
        printf("");
    }
	Team tempTeam(teamId, 0);

	if (!teamsInSystem.find(teamsInSystem.getRoot(), tempTeam))
	{
		return StatusType::FAILURE;
	}

    if(nonEmptyTeams.find(nonEmptyTeams.getRoot(), tempTeam))
    {
        return StatusType::FAILURE;
    }

	int numOfTeams = teamsInSystem.getNodesNum();
	teamsInSystem.remove(&tempTeam, true);

	if (teamsInSystem.getNodesNum() == numOfTeams)
	{
		return StatusType::FAILURE;
	}

	this->totalTeams--;

	return StatusType::SUCCESS;
}

StatusType world_cup_t::add_player(int playerId, int teamId, int gamesPlayed,
	int goals, int cards, bool goalKeeper)
{

	if (playerId <= 0 || teamId <= 0 || gamesPlayed < 0 || goals < 0 || cards < 0)
	{
		return StatusType::INVALID_INPUT;
	}

	if (gamesPlayed == 0 && (goals > 0 || cards > 0))
	{
		return StatusType::INVALID_INPUT;
	}

    if(playerId == 5168)
    {
        printf("");
    }
    Team d(teamId, 0);
    Node<Team>* team = teamsInSystem.find(teamsInSystem.getRoot(), d);//findTeam(teamId, false);

    if (team == nullptr)
    {
        return StatusType::FAILURE;
    }

	PlayerById newPlayerId(playerId, team->data->teamIdPtr(), gamesPlayed, goals, cards, goalKeeper);
	PlayerByStats newPlayerSt(playerId, team->data->teamIdPtr(), gamesPlayed, goals, cards, goalKeeper);
	std::shared_ptr<int> closest = std::make_shared<int>();
	*closest = -1;
	newPlayerSt.setClosestPtr(closest);

	//Adding the player to the players' trees
	if (playersById.find(playersById.getRoot(), newPlayerId)) //|| playersByStats.find(playersByStats.getRoot(), newPlayerSt))
	{
		return StatusType::FAILURE;
	}

	if (!playersByStats.insert(&newPlayerSt, true) || !playersById.insert(&newPlayerId, true))
	{
		return StatusType::FAILURE;
	}
	
	Node<PlayerByStats>* tempS = playersByStats.find(playersByStats.getRoot(), newPlayerSt);
	
	findClosest(tempS->next);
	findClosest(tempS->previous);
	findClosest(tempS);
	newPlayerSt = *playersByStats.find(playersByStats.getRoot(), newPlayerSt)->data;



	if (nonEmptyTeams.find(nonEmptyTeams.getRoot(), *(team->data)) == nullptr)
	{
		if (team->data->insertPlayer(playerId, gamesPlayed, goals, cards, goalKeeper) == false)
		{
			return StatusType::FAILURE;
		}

		nonEmptyTeams.insert(team->data, true);
		findTeam(teamId, true)->data->removePlayer(playerId);
	}
	else
	{
		Node<Team>* tempTeam = findTeam(teamId, false); //Find it in the nonEmptyTeams tree

		if (tempTeam->data->insertPlayer(playerId, gamesPlayed, goals, cards, goalKeeper) == false)
		{
			return StatusType::FAILURE;
		}
	}

	//Checking if the team is fit to join the active teams tree
	Node<Team>* tempTeam = findTeam(teamId, false);
	if (tempTeam->data->getPlayersCount() >= 11 && tempTeam->data->hasGoalKeeper())
	{
		Node<Team>* currentTeam = activeTeams.find(activeTeams.getRoot(), *team->data);
		if (currentTeam == nullptr)
			activeTeams.insert(tempTeam->data, true);
		else
		{
			currentTeam->data->insertPlayer(playerId, gamesPlayed, goals, cards, goalKeeper);
		}
	}

	if (topScorer[1] <= goals)
	{
		if (topScorer[1] == goals)
		{
            if (topScorer[2] == cards)
            {
                if (topScorer[0] < playerId)
                {
                    topScorer[0] = playerId;
                }
            }
            else if(topScorer[2] > cards)
            {
                topScorer[0] = playerId;
                topScorer[2] = cards;
            }
		}
		else if (topScorer[1] < goals)
		{
			topScorer[0] = playerId;
			topScorer[1] = goals;
            topScorer[2] = cards;
		}
	}

	return StatusType::SUCCESS;
}

StatusType world_cup_t::remove_player(int playerId)
{
	if (playerId <= 0)
	{
		return StatusType::INVALID_INPUT;
	}

    std::shared_ptr<int> x(0);
    PlayerById temp(playerId, x, 0, 0, 0, false);
	Node<PlayerById>* tempPlayer = playersById.find(playersById.getRoot(), temp);

    if(playerId == 12086)
    {
        printf("");
    }
	if (tempPlayer == nullptr)
	{
		return StatusType::FAILURE;
	}


	int teamIdOfPlayer = tempPlayer->data->getTeamId();

    std::shared_ptr<int> xy = std::make_shared<int>(0);
	PlayerByStats pStats(playerId, xy, tempPlayer->data->getGamesPlayed(), tempPlayer->data->getGoalsCount(), tempPlayer->data->getCardsCount(), tempPlayer->data->isGoalKeeper());
	Node<PlayerByStats>* tempSt = playersByStats.find(playersByStats.getRoot(), pStats);

	Node<PlayerByStats>* tempNext = tempSt->next;
	Node<PlayerByStats>* tempPrevious = tempSt->previous;
	PlayerByStats nextData;
	PlayerByStats prevData;
	if (tempNext != nullptr)
	{
		nextData = *(tempNext->data);
	}
	if (tempPrevious != nullptr)
	{
		prevData = *(tempPrevious->data);
	}

	int amountOfPlayers[2] = { 0 };

	amountOfPlayers[0] = playersById.getNodesNum();
	amountOfPlayers[1] = playersByStats.getNodesNum();

	playersById.remove(tempPlayer->data, true);
	playersByStats.remove(tempSt->data, true);

	if (playersById.getNodesNum() == amountOfPlayers[0] || playersByStats.getNodesNum() == amountOfPlayers[1])
	{
		return StatusType::FAILURE;
	}

	Node<Team>* currentTeam = findTeam(teamIdOfPlayer, false);

	if (currentTeam->data->removePlayer(playerId) == false)
	{
		return StatusType::FAILURE;
	}

	if (currentTeam != nullptr && currentTeam->data->getPlayersCount() == 0)
	{
		nonEmptyTeams.remove(currentTeam->data, true);
        return StatusType::SUCCESS;
	}

	if (currentTeam != nullptr)
	{
		currentTeam = activeTeams.find(activeTeams.getRoot(), *(currentTeam->data));
	}

	if (currentTeam != nullptr)
	{
		currentTeam->data->removePlayer(playerId);
		if (currentTeam->data->getPlayersCount() < 11 || !currentTeam->data->hasGoalKeeper())
		{
			activeTeams.remove(currentTeam->data, true);
		}
	}

	if (topScorer[0] == playerId)
	{
		PlayerByStats* top = playersByStats.findMax(playersByStats.getRoot());

		if (top != nullptr)
		{
			topScorer[0] = top->getPlayerId();
			topScorer[1] = top->getGoalsCount();
            topScorer[2] = top->getCardsCount();
		}
	}

	findClosest(playersByStats.find(playersByStats.getRoot(), nextData));
	findClosest(playersByStats.find(playersByStats.getRoot(), prevData));

	return StatusType::SUCCESS;
}

StatusType world_cup_t::update_player_stats(int playerId, int gamesPlayed,
	int scoredGoals, int cardsReceived)
{
	if (playerId <= 0 || gamesPlayed < 0 || scoredGoals < 0 || cardsReceived < 0)
	{
		return StatusType::INVALID_INPUT;
	}


    if(playerId == 35224)
    {
        printf("");
    }
    std::shared_ptr<int> x(0);
	PlayerById tempPlayer(playerId, x, gamesPlayed, scoredGoals, cardsReceived, false);
	Node<PlayerById>* currentPlayer = playersById.find(playersById.getRoot(), tempPlayer);

	if (currentPlayer == nullptr)
	{
		return StatusType::FAILURE;
	}

	PlayerById tempId(*currentPlayer->data);

    std::shared_ptr<int> y = std::make_shared<int>(0);
	PlayerByStats p(playerId, y, currentPlayer->data->getGamesPlayed(), currentPlayer->data->getGoalsCount(), currentPlayer->data->getCardsCount(), currentPlayer->data->isGoalKeeper());
	Node<PlayerByStats>* playerToUpdate = playersByStats.find(playersByStats.getRoot(), p);


	if (playerToUpdate == nullptr)
	{
		return StatusType::FAILURE;
	}

	PlayerByStats tempSt(*playerToUpdate->data);

	remove_player(playerToUpdate->data->getPlayerId());
	tempSt.updateStats(gamesPlayed, scoredGoals, cardsReceived);
	add_player(tempSt.getPlayerId(), tempSt.getTeamId(), tempSt.getGamesPlayed(), tempSt.getGoalsCount(), tempSt.getCardsCount(), tempSt.isGk());

	if (topScorer[1] <= tempSt.getGoalsCount())
	{
		if (topScorer[1] == tempSt.getGoalsCount())
		{
            if(topScorer[2] == tempSt.getCardsCount())
            {
                if (topScorer[0] < tempSt.getPlayerId())
                {
                    topScorer[0] = tempSt.getPlayerId();
                }
            }
            else if(topScorer[2] > tempSt.getCardsCount())
            {
                topScorer[0] = tempSt.getPlayerId();
                topScorer[2] = tempSt.getCardsCount();
            }
		}
		else if (topScorer[1] < tempSt.getGoalsCount())
		{
			topScorer[0] = tempSt.getPlayerId();
			topScorer[1] = tempSt.getGoalsCount();
            topScorer[2] = tempSt.getCardsCount();
		}
	}

	return StatusType::SUCCESS;
}

StatusType world_cup_t::play_match(int teamId1, int teamId2)
{
	if (teamId1 <= 0 || teamId2 <= 0 || teamId1 == teamId2)
	{
		return StatusType::INVALID_INPUT;
	}


	Team temp1(teamId1, 0);
	Team temp2(teamId2, 0);
	Node<Team>* activeTeam1 = activeTeams.find(activeTeams.getRoot(), temp1);
	Node<Team>* activeTeam2 = activeTeams.find(activeTeams.getRoot(), temp2);

	if (!activeTeam1 || !activeTeam2)
	{
		return StatusType::FAILURE;
	}

	if (activeTeam1->data->getTeamPower() < activeTeam2->data->getTeamPower())
	{
		activeTeam2->data->addPoints(3);
		findTeam(teamId2, false)->data->addPoints(3);
	}
	else if (activeTeam1->data->getTeamPower() > activeTeam2->data->getTeamPower())
	{
		activeTeam1->data->addPoints(3);
		findTeam(teamId1, false)->data->addPoints(3);
	}
	else
	{
		activeTeam1->data->addPoints(1);
		activeTeam2->data->addPoints(1);
		findTeam(teamId1, false)->data->addPoints(1);
		findTeam(teamId2, false)->data->addPoints(1);
	}

	activeTeam1->data->incrementGamesPlayed();
	activeTeam2->data->incrementGamesPlayed();

	return StatusType::SUCCESS;
}

output_t<int> world_cup_t::get_num_played_games(int playerId)
{
    if(playerId <= 0)
    {
        return output_t<int>(StatusType::INVALID_INPUT);
    }

    std::shared_ptr<int> y = std::make_shared<int>(0);
	PlayerById tempPlayer(playerId, y, 0, 0, 0, false);
	Node<PlayerById>* temp = playersById.find(playersById.getRoot(), tempPlayer);

	if (!temp)
	{
		output_t<int> output(StatusType::FAILURE);
		return output;
	}

	int gamesPlayed = (temp->data->getGamesPlayed());
	output_t<int> output(gamesPlayed);

	return output;
}

output_t<int> world_cup_t::get_team_points(int teamId)
{
	if (teamId <= 0)
	{
		return output_t<int>(StatusType::INVALID_INPUT);
	}

	Node<Team>* team = findTeam(teamId, true);
	int points = 0;

	if (!team && !(findTeam(teamId, true)))
	{
		return output_t<int>(StatusType::FAILURE);
	}

	if (findTeam(teamId, true) != nullptr)
	{
		points = findTeam(teamId, true)->data->getTeamPoints();
	}
	else if (team != nullptr)
	{
		points = team->data->getTeamPoints();
	}

	output_t<int> res(points);

	return res;
}

StatusType world_cup_t::unite_teams(int teamId1, int teamId2, int newTeamId)
{
	if (newTeamId <= 0 || teamId1 <= 0 || teamId2 <= 0)
	{
		return StatusType::INVALID_INPUT;
	}
	if (teamId1 == teamId2)
	{
		return StatusType::INVALID_INPUT;
	}

	if (!findTeam(teamId1, true) || !findTeam(teamId2, true))
	{
		return StatusType::FAILURE;
	}

	if (findTeam(newTeamId, true))
	{
		if (findTeam(newTeamId, true)->data->getID() != teamId1 && findTeam(newTeamId, true)->data->getID() != teamId2)
		{
			return StatusType::FAILURE;
		}
	}


    if(teamId2 == 42710)
    {
        printf("");
    }
	Node<Team>* team1 = findTeam(teamId1, false);
	Node<Team>* team2 = findTeam(teamId2, false);
    Team t1;
    Team t2;

    if(team1 != nullptr)
    {
        t1 = *team1->data;
    }
    if (team2 != nullptr)
    {
        t2 = *team2->data;
    }

	if (team2 == nullptr)
	{
		if (team1 != nullptr)
		{
			//team1->data->setNewId(newTeamId);

			Team activeTeamTemp(teamId1, 0);
			Node<Team>* activeTeam1 = activeTeams.find(activeTeams.getRoot(), activeTeamTemp);

            teamsInSystem.remove(&activeTeamTemp, true);
            nonEmptyTeams.remove(team1->data, true);
            activeTeamTemp.setNewId(newTeamId);
            teamsInSystem.insert(&activeTeamTemp, true);

            if (activeTeam1 != nullptr)
            {
                Team act = *activeTeam1->data;
                activeTeams.remove(activeTeam1->data, true);
                act.setNewId(newTeamId);
                activeTeams.insert(&act, true);
            }

            t1.setNewId(newTeamId);
            nonEmptyTeams.insert(&t1, true);



		}
		else
		{
            Team emptyTeamTemp(teamId1, 0);
            teamsInSystem.remove(&emptyTeamTemp, true);
            emptyTeamTemp.setNewId(newTeamId);
            teamsInSystem.insert(&emptyTeamTemp, true);
		}
		remove_team(teamId2);
	}
	else
	{
		Team activeTeamTemp2(teamId2, 0);
		Node<Team>* activeTeam2 = activeTeams.find(activeTeams.getRoot(), activeTeamTemp2);

		if (team1 == nullptr)
		{
			/*team2->data->setNewId(newTeamId);
			updateTeamId(team2->data->getRootOfIds(), newTeamId);
			findTeam(teamId2, true)->data->setNewId(newTeamId);*/
            team2->data->setNewId(newTeamId);
            activeTeamTemp2.setNewId(newTeamId);
            teamsInSystem.remove(&activeTeamTemp2, true);
            nonEmptyTeams.remove(team2->data, true);

            if (activeTeam2 != nullptr)
            {
                Team act = *activeTeam2->data;
                activeTeams.remove(activeTeam2->data, true);
                act.setNewId(newTeamId);
                activeTeams.insert(&act, true);
            }
            t2.setNewId(newTeamId);
            activeTeamTemp2.setNewId(newTeamId);
            teamsInSystem.insert(&activeTeamTemp2, true);
            nonEmptyTeams.insert(&t2, true);



			remove_team(teamId1);
		}
		else
		{
			Node<Team>* activeTeam = activeTeams.find(activeTeams.getRoot(), *team1->data);

			team1->data->mergeTeams(*team2->data);
            team2->data->setNewId(newTeamId);
			teamsInSystem.remove(findTeam(newTeamId, true)->data, true);
            nonEmptyTeams.remove(findTeam(newTeamId, false)->data, true);

            Team tempTeam(teamId1, 0);
            teamsInSystem.remove(&tempTeam, true);

            nonEmptyTeams.remove(&tempTeam, true);
            tempTeam.setNewId(newTeamId);
            teamsInSystem.insert(&tempTeam, true);

            Team s(t1);
            s.setNewId(newTeamId);
            nonEmptyTeams.insert(&s, true);

			//findTeam(teamId1, true)->data->setNewId(newTeamId);

			if (activeTeam2 != nullptr)
			{
				activeTeams.remove(activeTeam2->data, true);
			}


			if (activeTeam == nullptr && t1.getPlayersCount() >= 11 && t1.hasGoalKeeper())
			{
				activeTeams.insert(&s, true);
			}
			else if (activeTeam != nullptr)
			{
				activeTeams.remove(&s, true);
				activeTeams.insert(&s, true);
			}

			//nonEmptyTeams.remove(team2->data, true);
			this->totalTeams--;
		}
	}
	return StatusType::SUCCESS;
}

output_t<int> world_cup_t::get_top_scorer(int teamId)
{
	if (teamId == 0)
		return StatusType::INVALID_INPUT;
	if (teamId < 0)
	{
		if (playersById.getNodesNum() == 0)
			return StatusType::FAILURE;
		return output_t<int>(topScorer[0]);
	}
	else //teamId > 0
	{
		Node<Team>* currentTeam = findTeam(teamId, false);
		if (currentTeam == nullptr || currentTeam->data->getPlayersCount() == 0)
		{
			return StatusType::FAILURE;
		}
		return output_t<int>(currentTeam->data->getTopScorer());
	}
}

output_t<int> world_cup_t::get_all_players_count(int teamId)
{
	if (teamId == 0)
	{
		return output_t<int>(StatusType::INVALID_INPUT);
	}
	else if (teamId < 0)
	{
		int totalPlayers = playersById.getNodesNum();
		return output_t<int>(totalPlayers);
	}
	else //teamId > 0
	{
		Node<Team>* team = findTeam(teamId, false);
		int totalPlayersInTeam = 0;

		if (team == nullptr && findTeam(teamId, true) == nullptr)
		{
			return output_t<int>(StatusType::FAILURE);
		}
		else if (team != nullptr)
		{
			totalPlayersInTeam = team->data->getPlayersCount();
		}

		return output_t<int>(totalPlayersInTeam);
	}
}

StatusType world_cup_t::get_all_players(int teamId, int* const output)
{
	if (teamId == 0 || output == nullptr)
	{
		return StatusType::INVALID_INPUT;
	}
	else if (teamId < 0)
	{
		if (playersById.getNodesNum() == 0)
			return StatusType::FAILURE;
		int index = 0;
		PlayerByStats* const playersOutput = new PlayerByStats[playersByStats.getNodesNum()];
		playersByStats.inOrder(playersByStats.getRoot(), playersOutput, index);
		for (int i = 0; i < playersByStats.getNodesNum(); i++)
		{
			output[i] = playersOutput[i].getPlayerId();
		}
		delete[] playersOutput;
	}
	else //teamId > 0
	{
		Node<Team>* currentTeam = findTeam(teamId, false);
		if (currentTeam == nullptr || currentTeam->data->getPlayersCount() == 0)
		{
			return StatusType::FAILURE;
		}
		PlayerByStats* const playersOutput = new PlayerByStats[currentTeam->data->getPlayersCount()];
		currentTeam->data->inOrderPlayers(output, playersOutput);
		delete[] playersOutput;
	}
	return StatusType::SUCCESS;
}

output_t<int> world_cup_t::get_closest_player(int playerId, int teamId)
{
	if (playerId <= 0 || teamId <= 0)
		return StatusType::INVALID_INPUT;

    if(this->playersById.getNodesNum() == 1)
    {
        return StatusType::FAILURE;
    }


    if(!findTeam(teamId, true))
    {
        return StatusType::FAILURE;
    }

    std::shared_ptr<int> y = std::make_shared<int>(0);
	Node<Team>* team = findTeam(teamId, false);
    PlayerById pTemp(playerId, y, 0, 0, 0, false);
    if(!playersById.find(playersById.getRoot(), pTemp) || !team || playersById.find(playersById.getRoot(), pTemp)->data->getTeamId() != teamId)
    {
        return StatusType::FAILURE;
    }

	Node<PlayerByStats>* currentPlayer = team->data->findPlayerById(playerId);


	return currentPlayer->data->getClosest();
}

output_t<int> world_cup_t::knockout_winner(int minTeamId, int maxTeamId)
{
	if (minTeamId < 0 || maxTeamId < 0 || minTeamId > maxTeamId)
		return StatusType::INVALID_INPUT;



	Team* const participatingTeams = new Team[activeTeams.getNodesNum()];
	Team minTeam(minTeamId, 0);
	Team newMinTeam(minTeamId, 0);
	Node<Team>* minNode;

    if(minTeamId == 15367)
    {
        printf("");
    }
	if(!activeTeams.find(activeTeams.getRoot(),minTeam))
	{
		activeTeams.insert(&minTeam, true);

		if (activeTeams.find(activeTeams.getRoot(), minTeam)->next == nullptr)
        {
            activeTeams.remove(&minTeam, true);
            return StatusType::FAILURE;
        }

		newMinTeam = *activeTeams.find(activeTeams.getRoot(), minTeam)->next->data;

        if(newMinTeam.getID() > maxTeamId)
        {
            activeTeams.remove((&minTeam), true);
            return StatusType::FAILURE;
        }
		activeTeams.remove(&minTeam, true);
		minNode = activeTeams.find(activeTeams.getRoot(), newMinTeam);
	}
	else
	{
		minNode = activeTeams.find(activeTeams.getRoot(), minTeam);
	}

	Team maxTeam(maxTeamId, 0);
	Team newMaxTeam(maxTeamId, 0);

	Node<Team>* maxNode;
	if (!activeTeams.find(activeTeams.getRoot(), maxTeam))
	{
		activeTeams.insert(&maxTeam, true);
		if (activeTeams.find(activeTeams.getRoot(), maxTeam)->previous == nullptr)
        {
            activeTeams.remove(&maxTeam, true);
            return StatusType::FAILURE;
        }

		newMaxTeam = *activeTeams.find(activeTeams.getRoot(), maxTeam)->previous->data;

		activeTeams.remove(&maxTeam, true);
		maxNode = activeTeams.find(activeTeams.getRoot(), newMaxTeam);
        if(newMaxTeam.getID() < minTeamId)
        {
            return StatusType::FAILURE;
        }
	}
	else
	{
		maxNode = activeTeams.find(activeTeams.getRoot(), maxTeam);
	}
	int index = 0;
	Node<Team>* current = minNode;
	while (current != maxNode->next)
	{
		participatingTeams[index++] = *current->data;
        current = current->next;
	}
	if (index == 0)
	{
		return StatusType::FAILURE;
	}

	output_t<int> res = knockout_winner_aux(minTeamId, maxTeamId, participatingTeams, index);
	
	delete[] participatingTeams;

	return res;

}
output_t<int> world_cup_t::knockout_winner_aux(int minTeamId, int maxTeamId, Team* participatingTeams, int size)
{
	if (size == 1)
	{
		output_t<int> result(participatingTeams[0].getID());
		return result;
	}

	int j = 0;
	int firstResult = 0;
	int secondResult = 0;

	for (int i = 0; i < size; i += 2)
	{
		if (i % 2 == 0 && i == size - 1)
		{
			participatingTeams[j] = participatingTeams[i];
			break;
		}

		firstResult = participatingTeams[i].getTeamPower();
		secondResult = participatingTeams[i + 1].getTeamPower();

		if (firstResult > secondResult)
		{
			participatingTeams[i].addPoints(participatingTeams[i + 1].getTeamPower() + 3);
			participatingTeams[j] = participatingTeams[i];
		}
		else //draw or second team won
		{
			participatingTeams[i + 1].addPoints(participatingTeams[i].getTeamPower() + 3);
			participatingTeams[j] = participatingTeams[i + 1];
		}
		j++;
	}
	int winnerId;
	if (size % 2 == 0)
	{
		output_t<int> winner = knockout_winner_aux(minTeamId, maxTeamId, participatingTeams, j);
		winnerId = winner.ans();
	}
	else
	{
		output_t<int> winner = knockout_winner_aux(minTeamId, maxTeamId, participatingTeams, j + 1);
		winnerId = winner.ans();
	}

	return output_t<int>(winnerId);
}
