import sys, numpy, matplotlib
import matplotlib.pyplot as pyplot
from os.path import basename

if __name__ == '__main__':
    for fname in sys.argv[1:]:
        title = basename(fname).split('.')[0]
        data = numpy.genfromtxt(fname, delimiter=',', missing_values=set([0, '0']), names=True, autostrip=True, case_sensitive=True, deletechars='', dtype=None)
        plot = pyplot.figure().add_subplot(1,1,1)
        matplotlib.rcParams.update({'font.size': 10})
        ########################################################################################################################################
        plot.semilogx(data['Elements'], data['fixed[F]'],         linestyle='-',  color='#000000', label='fixed[F]')
        plot.semilogx(data['Elements'], data['fixed[$\ell$]'],    linestyle='-',  color='#999999', label='fixed[$\ell$]')
        plot.semilogx(data['Elements'], data['byte[F]'],          linestyle='-',  color='#ff0000', label='byte[F]')
        plot.semilogx(data['Elements'], data['byte[$\ell$]'],     linestyle='-',  color='#990000', label='byte[$\ell$]')
        plot.semilogx(data['Elements'], data['bit[F]'],           linestyle='-',  color='#00ff00', label='bit[F]')
        plot.semilogx(data['Elements'], data['bit[$\ell$]'],      linestyle='-',  color='#0000ff', label='bit[$\ell$]')
        ####################################
        plot.semilogx(data['Elements'], data['fixed[$12$]fixed'], linestyle='--', color='#000000', label='fixed[$12$]fixed')
        plot.semilogx(data['Elements'], data['byte[$12$]byte'],   linestyle='--', color='#999999', label='byte[$12$]byte')
        plot.semilogx(data['Elements'], data['bit[$12$]bit'],     linestyle='--', color='#ff0000', label='bit[$12$]bit')
        plot.semilogx(data['Elements'], data['fixed[$12$]byte'],  linestyle='--', color='#990000', label='fixed[$12$]byte')
        plot.semilogx(data['Elements'], data['fixed[$12$]bit'],   linestyle='--', color='#00ff00', label='fixed[$12$]bit')
        plot.semilogx(data['Elements'], data['byte[$12$]bit'],    linestyle='--', color='#0000ff', label='byte[$12$]bit')
     #  ####################################
     #  plot.semilogx(data['Elements'], data['fixed[$14$]fixed'], linestyle=':',  color='#000000', label='fixed[$14$]fixed')
     #  plot.semilogx(data['Elements'], data['byte[$14$]byte'],   linestyle=':',  color='#999999', label='byte[$14$]byte')
     #  plot.semilogx(data['Elements'], data['bit[$14$]bit'],     linestyle=':',  color='#ff0000', label='bit[$14$]bit')
     #  plot.semilogx(data['Elements'], data['fixed[$14$]byte'],  linestyle=':',  color='#990000', label='fixed[$14$]byte')
     #  plot.semilogx(data['Elements'], data['fixed[$14$]bit'],   linestyle=':',  color='#00ff00', label='fixed[$14$]bit')
     #  plot.semilogx(data['Elements'], data['byte[$14$]bit'],    linestyle=':',  color='#0000ff', label='byte[$14$]bit')
     #  ####################################
        plot.semilogx(data['Elements'], data['fixed[$16$]fixed'], linestyle='-.', color='#000000', label='fixed[$16$]fixed')
        plot.semilogx(data['Elements'], data['byte[$16$]byte'],   linestyle='-.', color='#999999', label='byte[$16$]byte')
        plot.semilogx(data['Elements'], data['bit[$16$]bit'],     linestyle='-.', color='#ff0000', label='bit[$16$]bit')
        plot.semilogx(data['Elements'], data['fixed[$16$]byte'],  linestyle='-.', color='#990000', label='fixed[$16$]byte')
        plot.semilogx(data['Elements'], data['fixed[$16$]bit'],   linestyle='-.', color='#00ff00', label='fixed[$16$]bit')
        plot.semilogx(data['Elements'], data['byte[$16$]bit'],    linestyle='-.', color='#0000ff', label='byte[$16$]bit')
     #  ####################################
     #  plot.semilogx(data['Elements'], data['fixed[$18$]fixed'], linestyle=':',  color='#000000', label='fixed[$18$]fixed')
     #  plot.semilogx(data['Elements'], data['byte[$18$]byte'],   linestyle=':',  color='#999999', label='byte[$18$]byte')
     #  plot.semilogx(data['Elements'], data['bit[$18$]bit'],     linestyle=':',  color='#ff0000', label='bit[$18$]bit')
     #  plot.semilogx(data['Elements'], data['fixed[$18$]byte'],  linestyle=':',  color='#990000', label='fixed[$18$]byte')
     #  plot.semilogx(data['Elements'], data['fixed[$18$]bit'],   linestyle=':',  color='#00ff00', label='fixed[$18$]bit')
     #  plot.semilogx(data['Elements'], data['byte[$18$]bit'],    linestyle=':',  color='#0000ff', label='byte[$18$]bit')
     #  ####################################
        plot.semilogx(data['Elements'], data['fixed[$20$]fixed'], linestyle=':',  color='#000000', label='fixed[$20$]fixed')
        plot.semilogx(data['Elements'], data['byte[$20$]byte'],   linestyle=':',  color='#999999', label='byte[$20$]byte')
        plot.semilogx(data['Elements'], data['bit[$20$]bit'],     linestyle=':',  color='#ff0000', label='bit[$20$]bit')
        plot.semilogx(data['Elements'], data['fixed[$20$]byte'],  linestyle=':',  color='#990000', label='fixed[$20$]byte')
        plot.semilogx(data['Elements'], data['fixed[$20$]bit'],   linestyle=':',  color='#00ff00', label='fixed[$20$]bit')
        plot.semilogx(data['Elements'], data['byte[$20$]bit'],    linestyle=':',  color='#0000ff', label='byte[$20$]bit')
        ########################################################################################################################################
        pyplot.title('Fenwick tree: {}'.format(title), fontsize='large')
        pyplot.xlabel('# elements', fontsize='medium')
        pyplot.ylabel('time (nS)', fontsize='medium')
        pyplot.legend(loc="upper left", bbox_to_anchor=[0, 1], ncol=4, shadow=True, fancybox=True, prop={'size': 10})
        ########################################################################################################################################
        #pyplot.savefig('{}/{}.{}'.format('./fenwickpdf', title, 'pdf'))
        pyplot.show()
