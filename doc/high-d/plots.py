import pandas
import seaborn

seaborn.set(style='ticks')
df = pandas.DataFrame.from_csv('m_cleaned.csv')
grid = seaborn.pairplot(df)
grid.savefig("ecx.png")


