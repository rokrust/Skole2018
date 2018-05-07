import plotly as py
import plotly.figure_factory as ff

df = [dict(Task="Machine 1", Start='0', Finish='2', Resource='Job 1'),
      dict(Task="Machine 2", Start='1', Finish='4', Resource='Job 2'),
      dict(Task="Machine 1", Start='4', Finish='5', Resource='Job 2')]

colors = {'Job 1': 'rgb(255, 0, 0)',
          'Job 2': 'rgb(0, 0, 255)'}


fig = ff.create_gantt(df, colors=colors, index_col='Resource', show_colorbar=True, group_tasks=True)
py.offline.plot(fig, filename='gantt-group-tasks-together.html')


