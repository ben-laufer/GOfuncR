\name{get_names}
\alias{get_names}
\title{
Get the full names of gene ontology categories given the IDs
}
\description{
Returns the full names and the domains of GO-categories given the GO-IDs, e.g. 'GO:0042254'.
By default the package's integrated GO-graph is used, but a custom ontology can be defined, too.

}
\usage{
get_names(go_ids, term_df = NULL, godir = NULL)
}
\arguments{
    \item{go_ids}{
        a character() vector of GO-IDs, e.g. c('GO:0051082', 'GO:0042254').
    }
    \item{term_df}{
        optional data.frame() with an ontology 'term' table.
        Alternative to the default integrated GO-graph or \code{godir}.
    }
    \item{godir}{
        optional character() specifying a directory that
        contains the ontology table 'term.txt'.
        Alternative to the default integrated GO-graph or \code{term_df}.
    }
}

\details{
    For details on how to use a custom ontology with
    \code{term_df} or \code{godir} please refer to the package's vignette.
    The advantage of \code{term_df} over \code{godir} is that the latter
    reads the file 'term.txt' from disk and therefore takes longer.
}

\value{
a data.frame() with three columns: go_id (character()), go_name (character()) and root_node (domain, character()).
}
\references{
[1] Ashburner, M. et al. (2000). Gene Ontology: tool for the unification of biology. Nature Genetics 25, 25-29.
}
\author{
Steffi Grote
}

\seealso{
\code{\link{get_ids}}\cr
\code{\link{get_child_nodes}}\cr
\code{\link{get_parent_nodes}}\cr
}

\examples{
## get the full names of three random GO-IDs
get_names(c('GO:0051082', 'GO:0042254', 'GO:0000109'))
}



