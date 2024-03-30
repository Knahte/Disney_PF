Hello, it's the dev!
I've made an app that plots the shortest route between several Dinsey Land Paris attractions... And that's about it...

And you might say, "Yes, but it's like TSP" And I'd say, "Yes."

It's like TSP except that the salesman, if he takes the shortest route, will have to wait more than 2h30 because he wants to take a photo with a princess, whereas if he had **anticipated** this he could have waited only 1h00. My algorithm counteracts these variations in waiting time. 
It doesn't return the shortest path in distance (it can do that anyway, if you like), but it does return the shortest path in time.

I've tried to make path generation as customizable as possible with :
- selecting the attractions that will be in the generated path
- choosing the time and starting point (hotels or car park / train station)
- define your walking speed for groups that are a bit slow (in my personal experience, you need to multiply your usual walking speed by 75% (I went from 6 km/h in the city to 4 or 5 in the park)
- Finally, for those who prefer to go it alone, there's the "Single-Rider" mode.
So yes, it looks like an ordinary TSP with ordinary Lin-Kernighan heuristics.


How to use it ?

It's very simple: select your settings, click on "Save Setting", then select the attractions you wish to visit, click on "Save Selection" and finally, when everything's ready, click on "Sart Generation". After a few seconds *poof*, a list of all the attractions appears, with :
[time of arrival at attraction] - [distance to attraction] - [name of attraction].

A little bonus: you'll find a .GPX (GPS eXchange Format) file of the path to take in [installation folder]/Output. You can then transfer it to a .GPX file reader for a visual representation of the path to be rerouted.

Please note!
This project works well in theory, but has several drawbacks in practice. The most important is the waiting time: it's very complicated to predict the waiting time for an attraction, so I've taken the maximum average waiting time from https://queue-times.com/parks/4 . A second drawback is that there is currently no planing for a snack at midday.

This project is still under development, and I'd like to improve the speed and quality of the simulation / QoL, for example by adding restaurants / variable waiting times with date and influence / connection to a real-time waiting time API.


In terms of compatibility, it works on Windows 10/11 for sure, but for the rest I don't own any other OS, so I haven't been able to test it...


And that's about all you need to know about the project, enjoy!
