
state : scouting {
  every(8) { turn!(-1) }
  move!
  on(food) {
    if (strength < 3) {
      eat!
    } else {
      switch(@harvesting)
    }
  }
  on(enemy) {
    if (strength < 3) { turn!(4) }
    else { attack! }
  }
}

state : harvesting {
  take!
  turn!(4)
  switch(@backtracing)
}

state : backtracing {
  move!
  on(home) {
    drop!
    switch(@scouting)
  }
}

