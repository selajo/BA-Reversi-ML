import csv, sys, argparse, os

#parser
parser = argparse.ArgumentParser(description='Process some integers.')
parser.add_argument('-c','--csv',type=str, required=True, help='csv file to be processed')
parser.add_argument('-i','--index',type=int, required=True, help="""column to convert
    	1 MCTS-Depth
    	2 MiniMax-Depth
    	3 Total moves player
    	4 Total moves enemy
    	5 Time per move
    	6 Total gaming time
    	7 Score player
    	8 Score Rollouts
    	9 Score Limits
    	10 Proved wins
    	11 Proved losses
    	12 Number of captured edges
    	13 Bonus
    	14 Win or loss""")
parser.add_argument('-o','--out',type=str, required=True, help='output file')
parser.add_argument('-oi','--outindex',type=str, required=True, help='column of output file')
parser.add_argument('-oc','--outcol',type=str, required=True, help='column of output file')
parser.add_argument('-s','--operation',type=str, required=True, help='operation to use: mean or sum')


if __name__ == '__main__':
	delimiter = '  '
	args = parser.parse_args()
	csv_file = csv.reader(open(args.csv))
	dist = 0
	i = 0

	for row in csv_file:
		row = row[0].split(';')
		if len(row) > 13 and args.index == 14:
			_dist = row[len(row)-1]
		else:
			_dist = row[args.index-1]
		try: 
			_dist = float(_dist)
		except ValueError:
			_dist = 0

		dist += _dist
		i += 1

	if args.operation == "mean":
		dist = dist / i

	dist = str(round(dist, 2))
	if not os.path.isfile(args.out):
		f = open(args.out, 'w')
		f.write("i")
		f.close()

	f = open(args.out, 'r')
	data = f.readlines()

	#create column if necessary
	row = data[0].split('\n')[0].split(delimiter)
	if not args.outcol in row:
		data[0] = data[0].split('\n')[0] + delimiter + args.outcol + '\n'


	found = False
	#write solution to file
	for row in data:
		listrow = row.split('\n')[0].split(delimiter)
		if listrow[0] == args.outindex:
			data[data.index(row)] = row.split('\n')[0] + delimiter + dist + '\n'
			found = True
	#append if index is not found
	if not found:
		newrow = args.outindex + delimiter + dist + '\n'
		data.append(newrow)

	f.close
	f = open(args.out, 'w')
	f.writelines(data)


