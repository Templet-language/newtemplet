/*$TET$$header*/

/*$TET$*/

#pragma templet !master(cw!number,sw!number)

struct master :public templet::actor {
	master(templet::engine&e):templet::actor(e) {
/*$TET$master$master*/
/*$TET$*/
	}

	void on_start(){
/*$TET$master$start*/
/*$TET$*/
	}

/*$TET$master$$footer*/
/*$TET$*/
};

#pragma templet cworker(cw?number,t:base.mytask)

struct cworker :public templet::actor {
	cworker(templet::engine&e):templet::actor(e) {
/*$TET$cworker$cworker*/
/*$TET$*/
	}

/*$TET$cworker$$footer*/
/*$TET$*/
};

#pragma templet sworker(sw?number,t:base)

struct sworker :public templet::actor {
	sworker(templet::engine&e):templet::actor(e) {
/*$TET$sworker$sworker*/
/*$TET$*/
	}

/*$TET$sworker$$footer*/
/*$TET$*/
};

/*$TET$$footer*/

/*$TET$*/
