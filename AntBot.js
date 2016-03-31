"use strict";

const fs = require('fs');
const execSync = require('child_process').execSync;

var segmentsList = ['ALPHA','BETA','Q_PHENO','REDUCE_PHENO','START_PHENO','PHENO_DECREASE','Q_DIST'];
var segmentsRange = {
	ALPHA : {min:0.1,max:2,step:0.2},
	BETA : {min:5,max:5,step:0.5},
	Q_PHENO : {min:10000,max:20000,step:1000},
	PHENO_DECREASE : {min:0.95,max:0.95,step:0.01},
	START_PHENO : {min:100,max:100,step:100},
	REDUCE_PHENO : {min:0.999,max:0.999,step:0.001},
	Q_DIST : {min:100,max:100,step:1}
};
var number = 0;
function dfs(segments,segmentsNum,max) {
	if (segmentsNum == segmentsList.length) {
		number ++;
		var outputString = "";
		var segString = "";
		for (var seg in segments) {
			outputString += seg + " " + segments[seg] + "\n";
			segString += seg + " " + segments[seg] + "\n";
		}
		fs.writeFileSync('arg', outputString);
		var sum=0;
		for (var i=0;i<max;i++) {
			var child = execSync('./bin/future_net case3/topo.csv case3/demand.csv result arg');
			var cost = fs.readFileSync('results');
			//console.log(cost.toString());
			if (parseInt(cost.toString()) == 99999) {
				sum = 0;
				console.log('fail');
				break;
			} else {
				console.log('su');
			}
			sum += parseInt(cost.toString());
		}
		if (sum !=0) {
			fs.appendFileSync('Answers' + max,segString + sum + "\n");
			console.log(segments);
			console.log(sum);
		}
	} else {
		for (var i=segmentsRange[segmentsList[segmentsNum]].min; i<=segmentsRange[segmentsList[segmentsNum]].max; i=i+segmentsRange[segmentsList[segmentsNum]].step) {
			segments[segmentsList[segmentsNum]] = i;
			dfs(segments,segmentsNum+1,max);
		}
	}
}

var segments={};

dfs(segments,0,3);

dfs(segments,0,5);

dfs(segments,0,10);
