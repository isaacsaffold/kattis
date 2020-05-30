def max_exchanged(charges):
    mu = sum(charges) / len(charges)
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
        for i in range(num_people):
            line = input()
            charges.append(int(line[:-3] + line[-2:]))
        num_people = int(input())
        print("${}.{:02}".format(*divmod(max_exchanged(charges), 100)))

if __name__ == "__main__":
    main()
