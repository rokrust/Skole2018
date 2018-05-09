import plotly as py
import plotly.figure_factory as ff
import pandas as pd
from gantt_functions import create_gantt
from os import system
import os
import sys

def plot_gantt(dir, filename):
    df = pd.read_csv(dir + "/" + filename + ".csv")
    # Need list of more colors, up to 20 (max number of jobs)?
    #colors = ['#333F44', '#93e4c1', '#660066', '#006666', '#008877', '#801010', '#808000', '#123456', '#F44333','#1A3AF0']
    colors = ["#ff0000","#ff8000","#ffff00","#80ff00", "#00ff00","#00ff80",
              "#00ffff","#0080ff","#4000ff","#bf00ff","#ff00bf","#ff0040"]

    fig = create_gantt(df, colors=colors, show_colorbar=True, index_col='Job',
                       bar_width=0.2, showgrid_x=True, showgrid_y=True, group_tasks=True)
    py.offline.plot(fig, filename=filename + '.html')

if __name__ == "__main__":
    optimal_value = 0
    if len(sys.argv) == 4:
        optimal_value = sys.argv[3]


    exe_path = "..\\Release\\Swarm_job_shop_scheduling.exe"
    problem_path = "..\\Test Data\\4.txt"
    print(system("%s \"%s\" \"%s\" \"%s\"" % (exe_path, sys.argv[1], sys.argv[2], optimal_value)))
    plot_gantt("..", "solution")