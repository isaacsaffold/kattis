from sys import stdin

def max_exchanged(charges, total):
    mu = total / len(charges)
    gt_mu, lt_mu = 0, 0
    for x in charges:
        diff = x - mu
        if diff > 0:
            gt_mu += int(diff)
        else:
            lt_mu += int(-diff)
    return max(gt_mu, lt_mu)

def main():
    num_people = int(input())
    while num_people:
        charges = []
        total = 0
        for i in range(num_people):
            line = stdin.readline()
            n = int(line[:-4] + line[-3:-1])
            charges.append(n)
            total += n
        num_people = int(input())
        print("${}.{:02}".format(*divmod(max_exchanged(charges, total), 100)))

if __name__ == "__main__":
    main()
