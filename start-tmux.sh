#!/bin/bash
SESSION=$USER

cd /home/pi/RaspberryPiDrone

tmux -2 new-session -d -s $SESSION

tmux new-window -t $SESSION:1 -n 'RaspberryPiDrone'

tmux split-window -v
tmux select-pane -t 0
tmux send-keys "cd  readSensors/; ./readSensors" C-m

tmux select-pane -t 1
tmux send-keys "cd  determineState/; ./determineState" C-m
tmux split-window -v

tmux select-pane -t 2
tmux send-keys "cd  vehicleControl/; ./vehicleControl" C-m
tmux split-window -v

tmux select-pane -t 3
tmux send-keys "cd  throttleControl/; ll" C-m

tmux resize-pane -t 0 -y 8
tmux resize-pane -t 1 -y 8
tmux resize-pane -t 2 -y 6

# Set default window
tmux select-window -t $SESSION:1

# Attach to session
tmux -2 attach-session -t $SESSION